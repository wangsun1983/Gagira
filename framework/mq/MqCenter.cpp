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

using namespace obotcha;

namespace gagira {

//------------MqStreamGroup-------
_MqStreamGroup::_MqStreamGroup(String ch) {
    mStreams = createConcurrentQueue<OutputStream>();
    mChannel = ch;
}

//------------MqCenter-------------
_MqCenter::_MqCenter(String url,int workers,int buffsize,int acktimeout,int retryTimes,int retryintervals) {

    monitor = createSocketMonitor();

    mAddrss = createHttpUrl(url)->getInetAddress()->get(0);
    
    mStreams = createConcurrentHashMap<String,MqStreamGroup>();

    mBuffer = createByteRingArray(buffsize);
    
    mReader = createByteRingArrayReader(mBuffer);

    mCurrentMsgLen = 0;

    mMutex = createMutex();
    waitExit = createCondition();

    mAckTimeout = acktimeout;

    mAckTimerFuture = createHashMap<String,Future>();

    mRetryInterval = retryintervals;
    mRetryTimes = retryTimes;

    mTimer = createThreadScheduledPoolExecutor();

    //if(inf == nullptr) {
    //    mPersistence = createMqDefaultPersistence();
    //    mPersistence->init();
    //}
    
    //create server socket
    mServerSock = createSocketBuilder()->setAddress(mAddrss)->newServerSocket();
    if(mServerSock->bind() < 0) {
        Trigger(InitializeException,"MqCenter socket bind failed,err is %s",strerror(errno));
    }
    
    if(monitor->bind(mServerSock,AutoClone(this)) < 0) {
        Trigger(InitializeException,"MqCenter monitor bind failed");
    }
}

void _MqCenter::onSocketMessage(int event,Socket sock,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Message:
            mBuffer->push(data);
            while(1) {
                int availableDataSize = mBuffer->getAvailDataSize();
                if(mCurrentMsgLen != 0) {
                    if(mCurrentMsgLen <= availableDataSize) {
                        mReader->move(mCurrentMsgLen);
                        ByteArray data = mReader->pop();
                        dispatchMessage(sock,data);
                        mCurrentMsgLen = 0;
                        continue;
                    }
                } else if(mReader->read<uint32_t>(mCurrentMsgLen) == st(ByteRingArrayReader)::Continue) {
                    //mReader->pop();
                    continue;
                }
                break;
            }
        break;

        case st(NetEvent)::Disconnect:
        //Do nothing
        break;
    }
}

void _MqCenter::dispatchMessage(Socket sock,ByteArray data) {
    auto msg = st(MqMessage)::generateMessage(data);
    msg->mSocket = sock;
    int result = 0;
    
    if(msg->isSubscribe()) {
        result = processSubscribe(msg);
    } else if(msg->isPublish()) {
        result = processPublish(msg);
    } else if(msg->isPublishOneShot()) {
        result = processOneshot(msg);
    } else if(msg->isUnSubscribe()) {
        result = processUnSubscribe(msg);
    }
    
    //if(msg->isPersist()) {
    //    if(result == 0) {
    //        mPersistence->onNewMessage(msg->channel,data,msg->flags);
    //    }
    //}

    //if(result == -1) {
    //    mPersistence->onFail(msg->channel,data,msg->flags);
    //}
}


int _MqCenter::close() {
    mStreams->clear();
    
    this->mTimer->shutdown();
    mTimer->awaitTermination();
    if(mServerSock != nullptr) {
        mServerSock->close();
        monitor->remove(mServerSock);
        mServerSock = nullptr;
    }

    if(monitor != nullptr) {
        monitor->close();
        monitor = nullptr;
    }
    
    waitExit->notify();
    return 0;
}

void _MqCenter::waitForExit(long interval) {
    AutoLock l(mMutex);
    waitExit->wait(mMutex,interval);
}

//process function
int _MqCenter::processAck(MqMessage msg) {
    //TODO
    return 0;
}

int _MqCenter::processPublish(MqMessage msg) {
    int result = -1;
    mStreams->syncReadAction([this,&msg,&result]{
        MqStreamGroup group = mStreams->get(msg->channel);
        if(group != nullptr) {
            auto ll = createArrayList<OutputStream>();
            auto packet = msg->generatePacket();
            ForEveryOne(stream,group->mStreams) {
                result = stream->write(packet);
                if(result < 0) {
                    ll->add(stream);
                }
            }

            if(ll->size() > 0) {
                group->mStreams->removeAll(ll);
            }
        }
    });

    return result;
}

int _MqCenter::processOneshot(MqMessage msg) {
    int result = -1;
    mStreams->syncReadAction([this,&msg,&result]{
        MqStreamGroup group = mStreams->get(msg->channel);
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
    mStreams->syncWriteAction([this,&msg]{
        MqStreamGroup group = mStreams->get(msg->channel);
        if(group == nullptr) {
            group = createMqStreamGroup(msg->getChannel());
            mStreams->put(msg->channel,group);
        }
        //check whether duplicated subscribe msg was sent
        auto outputStream = msg->mSocket->getOutputStream();
        ForEveryOne(stream,group->mStreams) {
            if(stream == outputStream) {
                return;
            }
        }
        group->mStreams->add(msg->mSocket->getOutputStream());
    });
    
    return 0;
}

int _MqCenter::processUnSubscribe(MqMessage msg) {
    MqMessage resp = createMqMessage(msg->getChannel(),nullptr,st(MqMessage)::Detach);

    mStreams->syncWriteAction([this,&msg,&resp]{
        MqStreamGroup group = mStreams->get(msg->channel);
        if(group != nullptr && group->mStreams != nullptr) {
            msg->mSocket->getOutputStream()->write(resp->generatePacket());
            group->mStreams->remove(msg->mSocket->getOutputStream());
            if(group->mStreams->size() == 0) {
                mStreams->remove(msg->channel);
            }
            return;
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
