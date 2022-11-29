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
#include "Log.hpp"


using namespace obotcha;

namespace gagira {

_MqCenter::_MqCenter(String url,MqOption option) {
    mSocketMonitor = createSocketMonitor();
    mAddress = createHttpUrl(url)->getInetAddress()->get(0);
    mChannelGroups = createConcurrentHashMap<String,ArrayList<OutputStream>>();
    mClients = createConcurrentHashMap<Socket,MqLinker>();
    mExitLatch = createCountDownLatch(1);
    mStickyMessages = createConcurrentHashMap<String,HashMap<String,ByteArray>>();
    mOption = (option == nullptr)?createMqOption():option;

    //create server socket
    mServerSock = createSocketBuilder()->setAddress(mAddress)->newServerSocket();

    mUuid = createUUID();

    mWaitAckThreadPools = createExecutorBuilder()->newScheduledThreadPool();
    mWaitAckMessages = createConcurrentHashMap<String,Future>();
}

int _MqCenter::start() {
    if(mServerSock->bind() < 0) {
        return -errno;
    }

    if(mSocketMonitor->bind(mServerSock,AutoClone(this)) < 0) {
        return -1;
    }

    return 0;
}

void _MqCenter::onSocketMessage(int event,Socket sock,ByteArray data) {
    auto client = mClients->get(sock);
    
    switch(event) {
        case st(NetEvent)::Connect: {
            client = createMqLinker(mOption->getClientRecvBuffSize());
            mClients->put(sock,client);
        }
        break;

        case st(NetEvent)::Message: {
            if(client == nullptr) {
                LOG(ERROR)<<"Received a message,but can not find it's connection";
                return;
            }

            auto parser = client->getParser();
            parser->pushData(data);
            ArrayList<ByteArray> result = parser->doParse();
            if(result != nullptr && result->size() != 0) {
                ForEveryOne(msg,result) {
                    dispatchMessage(sock,msg);
                }
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
    } else if(msg->isAck()) {
        return processAck(msg);
    }

    return -1;
}


int _MqCenter::close() {

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
    auto fu = mWaitAckMessages->get(msg->getToken());
    if(fu != nullptr) {
        fu->cancel();
    }

    return 0;
}

int _MqCenter::registWaitAckTask(MqMessage msg) {
    msg->setToken(mUuid->generate());
    auto fu = mWaitAckThreadPools->schedule(mOption->getAckTimeout(),[this,msg](){
        int retryTimes = msg->getRetryTimes();
        if(retryTimes < mOption->getReDeliveryTimes()) {
            msg->setRetryTimes(retryTimes + 1);
            if(msg->isPublish()) {
                processPublish(msg);
            } else if(msg->isPublishOneShot()) {
                processOneshot(msg);
            }
        }
    });

    mWaitAckMessages->put(msg->getToken(),fu);
    return 0;
}

int _MqCenter::processPublish(MqMessage msg) {
    mChannelGroups->syncReadAction([this,&msg]{
        auto channels = mChannelGroups->get(msg->getChannel());
        if(channels != nullptr && channels->size() != 0) {
            auto ll = createArrayList<OutputStream>();
            auto packet = msg->generatePacket();
            ForEveryOne(stream,channels) {
                if(stream->write(packet) < 0) {
                    ll->add(stream);
                }
            }

            if(ll->size() > 0) {
                channels->removeAll(ll);
            }
        }
    });

    if(msg->isAcknowledge() && msg->getRetryTimes() == 0) {
        registWaitAckTask(msg);
    }

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

        mStickyMessages->syncWriteAction([this,&saveMessage,&channel,&tag] {
            auto stickyMap = mStickyMessages->get(channel);
            if(stickyMap == nullptr) {
                stickyMap = createHashMap<String,ByteArray>();
                mStickyMessages->put(channel,stickyMap);
            }
            stickyMap->put(tag,saveMessage->generatePacket());
        });
    } else if(msg->isUnStick()){
        mStickyMessages->syncWriteAction([this,&channel,&tag] {
            auto stickyMap = mStickyMessages->get(channel);
            if(stickyMap != nullptr) {
                stickyMap->remove(tag);
            }
        });
    }

    return 0;
}

int _MqCenter::processOneshot(MqMessage msg) {
    int result = -1;
    mChannelGroups->syncReadAction([this,&msg,&result]{
        auto channels = mChannelGroups->get(msg->getChannel());
        if(channels != nullptr && channels->size() != 0) {
            int random = st(System)::currentTimeMillis()%channels->size();
            auto packet = msg->generatePacket();//TODO? donot generate again??
            result = channels->get(random)->write(packet);
        } else{
            //TODO? Need resend????
        }

        if(msg->isAcknowledge() && msg->getRetryTimes() == 0) {
            registWaitAckTask(msg);
        }
    });

    return result;
}

int _MqCenter::processSubscribe(MqMessage msg) {
    mChannelGroups->syncWriteAction([this,&msg]{
        auto channel = msg->getChannel();
        auto channels = mChannelGroups->get(channel);
        if(channels == nullptr) {
            channels = createArrayList<OutputStream>();
            mChannelGroups->put(channel,channels);
        }
        //check whether duplicated subscribe msg was sent
        auto outputStream = msg->mSocket->getOutputStream();
        ForEveryOne(stream,channels) {
            if(stream == outputStream) {
                return;
            }
        }
        channels->add(outputStream);

        //check sticky message
        mStickyMessages->syncReadAction([this,&channel,&msg,&outputStream] {
            auto stickyMap = mStickyMessages->get(channel);
            if(stickyMap != nullptr) {
                ForEveryOne(pair,stickyMap) {
                    if(outputStream->write(pair->getValue()) < 0) {
                        break;
                    }
                }
            }
        });
    });

    return 0;
}

int _MqCenter::processUnSubscribe(MqMessage msg) {
    mChannelGroups->syncWriteAction([this,&msg]{
        auto channel = msg->getChannel();
        auto channels = mChannelGroups->get(channel);
        if(channels != nullptr && channels->size() > 0) {
            MqMessage resp = createMqMessage(channel,nullptr,st(MqMessage)::Detach);
            auto outputstream = msg->mSocket->getOutputStream();

            outputstream->write(resp->generatePacket());
            channels->remove(outputstream);
            if(channels->size() == 0) {
                mChannelGroups->remove(channel);
            }
        }
    });

    return 0;
}

_MqCenter::~_MqCenter() {
    this->close();
}

}
