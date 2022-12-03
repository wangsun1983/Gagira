#include "MqCenter.hpp"
#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "NetEvent.hpp"
#include "MqMessage.hpp"
#include "ForEveryOne.hpp"
#include "System.hpp"
#include "ExecutorBuilder.hpp"
#include "Log.hpp"
#include "Inspect.hpp"

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
    Inspect(mServerSock->bind() < 0 
            || mSocketMonitor->bind(mServerSock,AutoClone(this)) < 0,
            -1);
    return 0;
}

void _MqCenter::onSocketMessage(int event,Socket sock,ByteArray data) {
    
    switch(event) {
        case st(NetEvent)::Connect: {
            auto client = createMqLinker(mOption->getClientRecvBuffSize());
            mClients->put(sock,client);
        }
        break;

        case st(NetEvent)::Message: {
            auto client = mClients->get(sock);
            if(client == nullptr) {
                LOG(ERROR)<<"Received a message,but can not find it's connection";
                return;
            }

            ArrayList<ByteArray> result = client->doParse(data);
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
    switch(msg->getType()) {
        case st(MqMessage)::Subscribe:
            return processSubscribe(msg);
        case st(MqMessage)::UnSubscribe:
            return processUnSubscribe(msg);
        case st(MqMessage)::Publish:
            return processPublish(msg);
        case st(MqMessage)::Ack:
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
    auto fu = mWaitAckMessages->get(msg->getAckToken());
    if(fu != nullptr) {
        fu->cancel();
    }

    return 0;
}

int _MqCenter::registWaitAckTask(MqMessage msg) {
    msg->setAckToken(mUuid->generate());
    auto fu = mWaitAckThreadPools->schedule(mOption->getAckTimeout(),[this,msg](){
        int retryTimes = msg->getRetryTimes();
        if(retryTimes < mOption->getReDeliveryTimes()) {
            msg->setRetryTimes(retryTimes + 1);
            processPublish(msg);
        }
    });

    mWaitAckMessages->put(msg->getAckToken(),fu);
    return 0;
}

int _MqCenter::processPublish(MqMessage msg) {
    if(msg->isUnStick()){
        mStickyMessages->syncWriteAction([this,&msg] {
            auto channel = msg->getChannel();
            auto stickyMap = mStickyMessages->get(channel);
            if(stickyMap != nullptr) {
                stickyMap->remove(msg->getStickToken());
            }
        });
        return 0;
    }
    if(msg->isAcknowledge()) {
        registWaitAckTask(msg);
    }

    mChannelGroups->syncReadAction([this,&msg]{
        auto channels = mChannelGroups->get(msg->getChannel());
        if(channels != nullptr && channels->size() != 0) {
           auto packet = msg->generatePacket();
            if(msg->isOneShot()) {
                int random = st(System)::currentTimeMillis()%channels->size();
                channels->get(random)->write(packet);
            } else {
                auto ll = createArrayList<OutputStream>();
                ForEveryOne(stream,channels) {
                    if(stream->write(packet) < 0) {
                        ll->add(stream);
                    }
                }
                

                if(ll->size() > 0) {
                    channels->removeAll(ll);
                }
            }
        }
    });
    if(msg->isStick()) {
        mStickyMessages->syncWriteAction([this,&msg] {
            auto channel = msg->getChannel();
            auto stickyMap = mStickyMessages->get(channel);
            if(stickyMap == nullptr) {
                stickyMap = createHashMap<String,ByteArray>();
                mStickyMessages->put(channel,stickyMap);
            }
            stickyMap->put(msg->getStickToken(),msg->generatePacket());
        });
    }

    return 0;
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
