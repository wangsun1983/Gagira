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
#include "MqSustainMessage.hpp"
#include "Synchronized.hpp"
#include "MqDLQMessage.hpp"
#include "Md.hpp"

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

    mPersistRwLock = createReadWriteLock();;
    mPersistRLock = mPersistRwLock->getReadLock();
    mPersistWLock = mPersistRwLock->getWriteLock();

    mPostBackCompleted = ((option != nullptr && option->getWaitPostBack())
                            ?false:true);

    mDlqMutex = createMutex();
    mSha = createSha(SHA_256);
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
            if(sock == mPersistenceClient) {
                AutoLock l(mPersistWLock);
                mPersistenceClient = nullptr;
            }
        }
        break;
    }
}

int _MqCenter::dispatchMessage(Socket sock,ByteArray data) {
    auto msg = st(MqMessage)::generateMessage(data);
    msg->mSocket = sock;

    {
        AutoLock l(mPersistRLock);
        if(sock != mPersistenceClient &&
           msg->isPersist() && 
           mPersistenceClient != nullptr) {
            mPersistenceClient->getOutputStream()->write(data);
        }
    }

    //check whether this message timed out
    long expireTime = msg->getExpireTime();
    if(expireTime != 0 && st(System)::currentTimeMillis() > expireTime) {
        //message timeout
        MqDLQMessage dlqMessage = createMqDLQMessage();
        dlqMessage->setCode(st(MqDLQMessage)::MessageTimeOut)
                    ->setData(data)
                    ->setPointTime(st(System)::currentTimeMillis())
                    ->setSrcAddress(msg->mSocket->getInetAddress()->getAddress());
        processSendFailMessage(dlqMessage,true);
        return -1;
    }
    
    if(mPersistenceClient != sock 
        && sock != nullptr
        && !mPostBackCompleted
        && msg->getType() != st(MqMessage)::SubscribePersistence) {
        //send sustain message to client
        MqSustainMessage sustainMsg = createMqSustainMessage(st(MqSustainMessage)::WaitForPostBack,nullptr);
        MqMessage sendData = createMqMessage(nullptr,sustainMsg->serialize(),st(MqMessage)::Sustain);
        sock->getOutputStream()->write(sendData->generatePacket());
        return -1;
    }

    switch(msg->getType()) {
        case st(MqMessage)::Subscribe:
            return processSubscribe(msg);
        case st(MqMessage)::UnSubscribe:
            return processUnSubscribe(msg);
        case st(MqMessage)::Publish:
            return processPublish(msg);
        case st(MqMessage)::Ack:
            return processAck(msg);
        case st(MqMessage)::SubscribePersistence:
            return processSubscribePersistence(msg);
        case st(MqMessage)::PostBack:
            return processPostBack(msg);
        case st(MqMessage)::SubscribeDLQ:
            return processSubscribeDLQ(msg);
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
    Inspect(msg->mSocket == nullptr,-1);
    auto fu = mWaitAckMessages->get(msg->getAckToken());
    if(fu != nullptr) {
        fu->cancel();
    }

    return 0;
}

int _MqCenter::processPostBack(MqMessage msg) {
    if(msg->isComplete()) {
        mPostBackCompleted = true;
    }

    dispatchMessage(nullptr,msg->getData());
    return 0;
}

int _MqCenter::processSubscribePersistence(MqMessage msg) {
    Inspect(msg->mSocket == nullptr,-1);

    AutoLock l(mPersistWLock);
    mPersistenceClient = msg->mSocket;
    return 0;
}

int _MqCenter::processSubscribeDLQ(MqMessage msg) {
    AutoLock l(mDlqMutex);
    mDlqClient = msg->mSocket;
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

    if(msg->mSocket != nullptr) {
        mChannelGroups->syncReadAction([this,&msg]{
            auto channels = mChannelGroups->get(msg->getChannel());
            if(channels != nullptr && channels->size() != 0) {
                auto packet = msg->generatePacket();
                auto originStream = msg->mSocket->getOutputStream();
                if(msg->isOneShot()) {
                    int random = st(System)::currentTimeMillis()%channels->size();
                    auto stream = channels->get(random);
                    if(originStream == stream) {
                        if(channels->size() > 1) {
                            random = (random >= channels->size() -1)?random = 0:random++;
                            stream = channels->get(random);
                        }
                    }

                    if(stream == originStream || stream->write(packet) < 0) {
                        MqDLQMessage dlqMessage = createMqDLQMessage();
                        dlqMessage->setCode(stream == originStream?st(MqDLQMessage)::NoClient:st(MqDLQMessage)::ClientDisconnect)
                                  ->setData(packet)
                                  ->setPointTime(st(System)::currentTimeMillis())
                                  ->setSrcAddress(msg->mSocket->getInetAddress()->getAddress()); //TODO
                        processSendFailMessage(dlqMessage,true);
                    }
                } else {
                    auto ll = createArrayList<OutputStream>();
                    ForEveryOne(stream,channels) {
                        String token = nullptr;
                        if(stream != originStream && stream->write(packet) < 0) {
                            ll->add(stream);
                            MqDLQMessage dlqMessage = createMqDLQMessage();
                            auto s = Cast<SocketOutputStream>(stream);
                            dlqMessage->setCode(st(MqDLQMessage)::ClientDisconnect)
                                    ->setData(token == nullptr?packet:nullptr)
                                    ->setSrcAddress(msg->mSocket->getInetAddress()->getAddress())
                                    ->setDestAddress(s->getSocket()->getInetAddress()->getAddress())
                                    ->setPointTime(st(System)::currentTimeMillis())
                                    ->setToken(token);
                            token = processSendFailMessage(dlqMessage,token == nullptr);
                        }
                    }
                    
                    if(ll->size() > 0) {
                        channels->removeAll(ll);
                    }
                }
            } else {
                MqDLQMessage dlqMessage = createMqDLQMessage();
                dlqMessage->setCode(st(MqDLQMessage)::NoClient)
                            ->setData(msg->generatePacket())
                            ->setPointTime(st(System)::currentTimeMillis())
                            ->setSrcAddress(msg->mSocket->getInetAddress()->getAddress()); //TODO
                processSendFailMessage(dlqMessage,true);
            }
        });
    }

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
    Inspect(msg->mSocket == nullptr,-1);

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
    Inspect(msg->mSocket == nullptr,-1);

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

String _MqCenter::processSendFailMessage(MqDLQMessage msg,bool isNeedGenToken) {
    String token = msg->getToken();
    Synchronized(mDlqMutex){
        Inspect(mDlqClient == nullptr,token);
        if(isNeedGenToken) {
            long time = st(System)::currentTimeMillis();
            token = mSha->encrypt(mUuid->generate()->append(createString(time)));
            msg->setToken(token);
        }
        MqMessage resp = createMqMessage(nullptr,msg->serialize(),st(MqMessage)::Publish);
        mDlqClient->getOutputStream()->write(resp->generatePacket());
    }
    
    return token;
}

_MqCenter::~_MqCenter() {
    this->close();
}

}
