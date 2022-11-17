#include <thread>
#include <atomic>
#include <mutex>

#include "MqCenter.hpp"
#include "HttpUrl.hpp"
#include "ServerSocket.hpp"
#include "SocketBuilder.hpp"
#include "NetEvent.hpp"
#include "MqMessage.hpp"
#include "ByteArrayReader.hpp"
#include "ByteArrayWriter.hpp"
#include "InitializeException.hpp"
#include "MqDefaultPersistence.hpp"
#include "ForEveryOne.hpp"
#include "ArrayList.hpp"
#include "System.hpp"
#include "ExecutorBuilder.hpp"

using namespace obotcha;

namespace gagira {

//------------MqStreamGroup-------
_MqChannelGroup::_MqChannelGroup(String ch) {
    mStreams = createConcurrentQueue<OutputStream>();
    mChannel = ch;
}

//------------MqClientInfo-------
_MqClientInfo::_MqClientInfo(int buffsize){
    mBuffer = createByteRingArray(buffsize);
    mReader = createByteRingArrayReader(mBuffer);
    mCurrentMsgLen = 0;
}

//------------MqCenter-------------
_MqCenter::_MqCenter(String url,MqOption option) {
    mSocketMonitor = createSocketMonitor();
    mAddrss = createHttpUrl(url)->getInetAddress()->get(0);
    mChannelGroups = createConcurrentHashMap<String,MqChannelGroup>();
    mClients = createConcurrentHashMap<Socket,MqClientInfo>();
    mExitLatch = createCountDownLatch(1);
    mAckTimerFuture = createHashMap<String,Future>();
    mStickyMutex = createMutex();
    mStickyMessages = createHashMap<String,HashMap<String,ByteArray>>();
    mTimer = createExecutorBuilder()->newScheduledThreadPool();
    mOption = option;

    //create server socket
    mServerSock = createSocketBuilder()->setAddress(mAddrss)->newServerSocket();
    if(mServerSock->bind() < 0) {
        Trigger(InitializeException,"MqCenter socket bind failed,err is %s",strerror(errno));
    }

    if(mSocketMonitor->bind(mServerSock,AutoClone(this)) < 0) {
        Trigger(InitializeException,"MqCenter socket monitor bind failed");
    }
}

void _MqCenter::onSocketMessage(int event,Socket sock,ByteArray data) {
    auto client = mClients->get(sock);
    if(client == nullptr) {
        client = createMqClientInfo(mOption->getClientRecvBuffSize());
        mClients->put(sock,client);
    }

    auto buffer = client->mBuffer;
    auto reader = client->mReader;
    auto &msgLen = client->mCurrentMsgLen;

    switch(event) {
        case st(NetEvent)::Message: {
            buffer->push(data);
            while(1) {
                int availableDataSize = buffer->getAvailDataSize();
                if(msgLen != 0) {
                    if(msgLen <= availableDataSize) {
                        reader->move(msgLen);
                        ByteArray data = reader->pop();
                        dispatchMessage(sock,data);
                        msgLen = 0;
                        continue;
                    }
                } else if(reader->read<uint32_t>(msgLen)
                          == st(ByteRingArrayReader)::Continue) {
                    continue;
                }
                break;
            }
        }
        break;

        case st(NetEvent)::Disconnect: {
            mClients->remove(sock);
        }
        break;
    }
}

int _MqCenter::dispatchMessage(Socket sock,ByteArray data) {
    auto msg = st(MqMessage)::generateMessage(data);
    msg->mSocket = sock;

    if(msg->isSubscribe()) {
        return processSubscribe(msg);
    } else if(msg->isPublish()) {
        return processPublish(msg);
    } else if(msg->isPublishOneShot()) {
        return processOneshot(msg);
    } else if(msg->isUnSubscribe()) {
        return processUnSubscribe(msg);
    }

    return -1;
}


int _MqCenter::close() {
    this->mTimer->shutdown();
    mTimer->awaitTermination();

    if(mServerSock != nullptr) {
        mServerSock->close();
        mSocketMonitor->unbind(mServerSock);
        mServerSock = nullptr;
    }

    if(mSocketMonitor != nullptr) {
        mSocketMonitor->close();
        mSocketMonitor = nullptr;
    }

    mChannelGroups->clear();
    mClients->clear();

    mExitLatch->countDown();
    return 0;
}

void _MqCenter::waitForExit(long interval) {
    mExitLatch->await(interval);
}

//process function
int _MqCenter::processAck(MqMessage msg) {
    //TODO
    return 0;
}

int _MqCenter::processPublish(MqMessage msg) {
    mChannelGroups->syncReadAction([this,&msg]{
        MqChannelGroup group = mChannelGroups->get(msg->getChannel());
        if(group != nullptr && group->mStreams->size() != 0) {
            auto ll = createArrayList<OutputStream>();
            auto packet = msg->generatePacket();
            ForEveryOne(stream,group->mStreams) {
                if(stream->write(packet) < 0) {
                    ll->add(stream);
                }
            }

            if(ll->size() > 0) {
                group->mStreams->removeAll(ll);
            }
        }
    });

    return processStick(msg);
}

int _MqCenter::processStick(MqMessage msg) {
    String tag = msg->getStickTag();
    String channel = msg->getChannel();
    if(msg->isStick()) {
        auto saveMessage = createMqMessage(channel,
                                           tag,
                                           msg->getData(),
                                           st(MqMessage)::Stick);
        {
            AutoLock l(mStickyMutex);
            auto stickyMap = mStickyMessages->get(channel);
            if(stickyMap == nullptr) {
                stickyMap = createHashMap<String,ByteArray>();
                mStickyMessages->put(channel,stickyMap);
            }
            stickyMap->put(tag,saveMessage->generatePacket());
        }
    } else if(msg->isUnStick()){
        AutoLock l(mStickyMutex);
        auto stickyMap = mStickyMessages->get(channel);
        if(stickyMap != nullptr) {
            stickyMap->remove(tag);
        }
    }

    return 0;
}

int _MqCenter::processOneshot(MqMessage msg) {
    int result = -1;
    mChannelGroups->syncReadAction([this,&msg,&result]{
        MqChannelGroup group = mChannelGroups->get(msg->getChannel());
        if(group != nullptr) {
            group->mStreams->syncReadAction([&msg,&result,&group]{
                if(group->mStreams->size() > 0) {
                    int random = st(System)::currentTimeMillis()%group->mStreams->size();
                    auto packet = msg->generatePacket();//TODO? donot generate again??
                    result = group->mStreams->get(random)->write(packet);
                }
            });
        } else{
            //TODO? Need resend????
        }
    });

    return result;
}

int _MqCenter::processSubscribe(MqMessage msg) {
    mChannelGroups->syncWriteAction([this,&msg]{
        auto channel = msg->getChannel();
        MqChannelGroup group = mChannelGroups->get(channel);
        if(group == nullptr) {
            group = createMqChannelGroup(channel);
            mChannelGroups->put(channel,group);
        }
        //check whether duplicated subscribe msg was sent
        auto outputStream = msg->mSocket->getOutputStream();
        ForEveryOne(stream,group->mStreams) {
            if(stream == outputStream) {
                return;
            }
        }
        group->mStreams->add(msg->mSocket->getOutputStream());

        //check sticky message
        {
            AutoLock l(mStickyMutex);
            auto stickyMap = mStickyMessages->get(channel);
            if(stickyMap != nullptr) {
                ForEveryOne(pair,stickyMap) {
                    msg->mSocket->getOutputStream()->write(pair->getValue());
                }
            }
        }
    });

    return 0;
}

int _MqCenter::processUnSubscribe(MqMessage msg) {
    mChannelGroups->syncWriteAction([this,&msg]{
        auto channel = msg->getChannel();
        MqChannelGroup group = mChannelGroups->get(channel);
        if(group != nullptr && group->mStreams != nullptr) {
            MqMessage resp = createMqMessage(channel,nullptr,st(MqMessage)::Detach);
            auto outputstream = msg->mSocket->getOutputStream();

            outputstream->write(resp->generatePacket());
            group->mStreams->remove(outputstream);
            if(group->mStreams->size() == 0) {
                mChannelGroups->remove(msg->channel);
            }
        }
    });

    return 0;
}


int _MqCenter::setAcknowledgeTimer(MqMessage msg) {
    return 0;
}

_MqCenter::~_MqCenter() {
    this->close();
}

}
