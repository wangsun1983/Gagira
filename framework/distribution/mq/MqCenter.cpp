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

_MqCenter::_MqCenter(String url,DistributeOption option):_DistributeCenter(url,option) {
    mChannelGroups = createConcurrentHashMap<String,ArrayList<OutputStream>>();
    mStickyMessages = createConcurrentHashMap<String,HashMap<String,ByteArray>>();
    mWaitAckThreadPools = createExecutorBuilder()->newScheduledThreadPool();
    mWaitAckMessages = createConcurrentHashMap<String,Future>();
    mPersistRwLock = createReadWriteLock();;
    mPersistRLock = mPersistRwLock->getReadLock();
    mPersistWLock = mPersistRwLock->getWriteLock();
    mPostBackCompleted = ((option != nullptr && option->getWaitPostBack())
                            ?false:true);
    mSchedulePool = createThreadScheduledPoolExecutor(-1,1000);
    mDlqMutex = createMutex();
}

int _MqCenter::start() {
    return st(DistributeCenter)::start();
}

int _MqCenter::onMessage(DistributeLinker linker,ByteArray data) {
    dispatchMessage(linker->getSocket(),data);
    return 0;
}

int _MqCenter::onNewClient(DistributeLinker linker) {
    //doNothing
    return 0;
}

int _MqCenter::onDisconnectClient(DistributeLinker linker) {
    auto sock = linker->getSocket();
    if(sock == mPersistenceClient) {
        AutoLock l(mPersistWLock);
        mPersistenceClient = nullptr;
    } else if(sock == mDlqClient){
        AutoLock l(mDlqMutex);
        mDlqClient = nullptr;
    }

    return 0;
}

int _MqCenter::dispatchMessage(Socket sock,ByteArray data) {
    //auto msg = st(MqMessage)::generateMessage(data);
    auto msg = mConverter->generateMessage<MqMessage>(data);
    msg->mSocket = sock;

    long currentTime = st(System)::currentTimeMillis();
    long publishTime = msg->getPublishTime();
    if(publishTime != 0 && publishTime > currentTime ) {
        mSchedulePool->schedule(publishTime - currentTime ,
            [this](Socket sock,ByteArray data){
                dispatchMessage(sock,data);
        },sock,data);
        return -1;
    }
    
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
        processSendFailMessage(dlqMessage);
        return -1;
    }
    if(mPersistenceClient != sock 
        && sock != nullptr
        && !mPostBackCompleted
        && msg->getType() != st(MqMessage)::SubscribePersistence) {
        //send sustain message to client
        MqSustainMessage sustainMsg = createMqSustainMessage(st(MqSustainMessage)::WaitForPostBack,nullptr);
        MqMessage sendData = createMqMessage(nullptr,sustainMsg->serialize(),st(MqMessage)::Sustain);
        sock->getOutputStream()->write(mConverter->generatePacket(sendData));
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
    mChannelGroups->clear();
    st(DistributeCenter)::close();
    return 0;
}

//process function
int _MqCenter::processAck(MqMessage msg) {
    Inspect(msg->mSocket == nullptr,-1);
    auto fu = mWaitAckMessages->get(msg->getToken());
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
    //msg->setAckToken(mUuid->generate());
    auto fu = mWaitAckThreadPools->schedule(getOption()->getAckTimeout(),[this,msg](){
        int retryTimes = msg->getRetryTimes();
        if(retryTimes < getOption()->getReDeliveryTimes()) {
            msg->setRetryTimes(retryTimes + 1);
            processPublish(msg);
        }
    });

    mWaitAckMessages->put(msg->getToken(),fu);
    return 0;
}

int _MqCenter::processPublish(MqMessage msg) {
    if(msg->isUnStick()){
        mStickyMessages->syncWriteAction([this,&msg] {
            auto channel = msg->getChannel();
            auto stickyMap = mStickyMessages->get(channel);
            if(stickyMap != nullptr) {
                stickyMap->remove(msg->getToken());
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
                auto packet = mConverter->generatePacket(msg);
                auto originStream = msg->mSocket->getOutputStream();
                if(msg->isOneShot()) {
                    int random = st(System)::currentTimeMillis()%channels->size();
                    auto stream = channels->get(random);
                    if(originStream == stream) {
                        if(channels->size() > 1) {
                            (random >= channels->size() -1)?random = 0:random++;
                            stream = channels->get(random);
                        }
                    }

                    if(stream == originStream || stream->write(packet) < 0) {
                        MqDLQMessage dlqMessage = createMqDLQMessage();
                        dlqMessage->setCode(stream == originStream?st(MqDLQMessage)::NoClient:st(MqDLQMessage)::ClientDisconnect)
                                  ->setData(packet)
                                  ->setToken(msg->getToken())
                                  ->setPointTime(st(System)::currentTimeMillis())
                                  ->setSrcAddress(msg->mSocket->getInetAddress()->getAddress());
                        processSendFailMessage(dlqMessage);
                    }
                } else {
                    auto ll = createArrayList<OutputStream>();
                    bool isFirst = true;
                    ForEveryOne(stream,channels) {
                        if(stream != originStream && stream->write(packet) < 0) {
                            ll->add(stream);
                            MqDLQMessage dlqMessage = createMqDLQMessage();
                            auto s = Cast<SocketOutputStream>(stream);
                            dlqMessage->setCode(st(MqDLQMessage)::ClientDisconnect)
                                    ->setData(isFirst?packet:nullptr)
                                    ->setSrcAddress(msg->mSocket->getInetAddress()->getAddress())
                                    ->setDestAddress(s->getSocket()->getInetAddress()->getAddress())
                                    ->setPointTime(st(System)::currentTimeMillis())
                                    ->setToken(msg->getToken());
                            processSendFailMessage(dlqMessage);
                            isFirst = false;
                        }
                    }
                    
                    if(ll->size() > 0) {
                        channels->removeAll(ll);
                    }
                }
            } else {
                MqDLQMessage dlqMessage = createMqDLQMessage();
                dlqMessage->setCode(st(MqDLQMessage)::NoClient)
                            ->setData(mConverter->generatePacket(msg))
                            ->setPointTime(st(System)::currentTimeMillis())
                            ->setSrcAddress(msg->mSocket->getInetAddress()->getAddress())
                            ->setToken(msg->getToken());
                processSendFailMessage(dlqMessage);
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
            stickyMap->put(msg->getToken(),mConverter->generatePacket(msg));
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

            outputstream->write(mConverter->generatePacket(resp));
            channels->remove(outputstream);
            if(channels->size() == 0) {
                mChannelGroups->remove(channel);
            }
        }
    });

    return 0;
}

bool _MqCenter::processSendFailMessage(MqDLQMessage msg) {
    String token = msg->getToken();
    Synchronized(mDlqMutex){
        Inspect(mDlqClient == nullptr,false);
        MqMessage resp = createMqMessage(nullptr,msg->serialize(),st(MqMessage)::Publish);
        return mDlqClient->getOutputStream()->write(mConverter->generatePacket(resp)) > 0;
    }

    return false;
}

_MqCenter::~_MqCenter() {
    this->close();
}

}
