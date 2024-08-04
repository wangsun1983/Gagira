#include "BroadcastCenter.hpp"
#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "NetEvent.hpp"
#include "BroadcastMessage.hpp"
#include "ForEveryOne.hpp"
#include "System.hpp"
#include "ExecutorBuilder.hpp"
#include "Log.hpp"
#include "Inspect.hpp"
#include "BroadcastSustainMessage.hpp"
#include "Synchronized.hpp"
#include "BroadcastDLQMessage.hpp"
#include "Md.hpp"

using namespace obotcha;

namespace gagira {

_BroadcastCenter::_BroadcastCenter(String url,DistributeOption option):_DistributeCenter(url,option) {
    mChannelGroups = ConcurrentHashMap<String,ArrayList<OutputStream>>::New();
    mStickyMessages = ConcurrentHashMap<String,HashMap<String,ByteArray>>::New();
    mWaitAckThreadPools = ExecutorBuilder::New()->newScheduledThreadPool();
    mWaitAckMessages = ConcurrentHashMap<String,Future>::New();
    mPersistRwLock = ReadWriteLock::New();;
    mPersistRLock = mPersistRwLock->getReadLock();
    mPersistWLock = mPersistRwLock->getWriteLock();
    mPostBackCompleted = ((option != nullptr && option->getWaitPostBack())
                            ?false:true);
    mSchedulePool = ThreadScheduledPoolExecutor::New(-1,1000);
    mDlqMutex = Mutex::New();
}

int _BroadcastCenter::start() {
    return st(DistributeCenter)::start();
}

int _BroadcastCenter::onMessage(DistributeLinker linker,ByteArray data) {
    dispatchMessage(linker->getSocket(),data);
    return 0;
}

int _BroadcastCenter::onNewClient(DistributeLinker linker) {
    //doNothing
    return 0;
}

int _BroadcastCenter::onDisconnectClient(DistributeLinker linker) {
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

int _BroadcastCenter::dispatchMessage(Socket sock,ByteArray data) {
    //auto msg = st(BroadcastMessage)::generateMessage(data);
    auto msg = mConverter->generateMessage<BroadcastMessage>(data);
    msg->mSocket = sock;

    long currentTime = st(System)::CurrentTimeMillis();
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
    if(expireTime != 0 && st(System)::CurrentTimeMillis() > expireTime) {
        //message timeout
        BroadcastDLQMessage dlqMessage = BroadcastDLQMessage::New();
        dlqMessage->setCode(st(BroadcastDLQMessage)::MessageTimeOut)
                    ->setData(data)
                    ->setPointTime(st(System)::CurrentTimeMillis())
                    ->setSrcAddress(msg->mSocket->getInetAddress()->getAddress());
        processSendFailMessage(dlqMessage);
        return -1;
    }
    if(mPersistenceClient != sock 
        && sock != nullptr
        && !mPostBackCompleted
        && msg->getType() != st(BroadcastMessage)::SubscribePersistence) {
        //send sustain message to client
        BroadcastSustainMessage sustainMsg = BroadcastSustainMessage::New(st(BroadcastSustainMessage)::WaitForPostBack,nullptr);
        BroadcastMessage sendData = BroadcastMessage::New(nullptr,sustainMsg->serialize(),st(BroadcastMessage)::Sustain);
        sock->getOutputStream()->write(mConverter->generatePacket(sendData));
        return -1;
    }
    
    switch(msg->getType()) {
        case st(BroadcastMessage)::Subscribe:
            return processSubscribe(msg);
        case st(BroadcastMessage)::UnSubscribe:
            return processUnSubscribe(msg);
        case st(BroadcastMessage)::Publish:
            return processPublish(msg);
        case st(BroadcastMessage)::Ack:
            return processAck(msg);
        case st(BroadcastMessage)::SubscribePersistence:
            return processSubscribePersistence(msg);
        case st(BroadcastMessage)::PostBack:
            return processPostBack(msg);
        case st(BroadcastMessage)::SubscribeDLQ:
            return processSubscribeDLQ(msg);
    }
    return -1;
}


int _BroadcastCenter::close() {
    mChannelGroups->clear();
    st(DistributeCenter)::close();
    return 0;
}

//process function
int _BroadcastCenter::processAck(BroadcastMessage msg) {
    Inspect(msg->mSocket == nullptr,-1);
    auto fu = mWaitAckMessages->get(msg->getToken());
    if(fu != nullptr) {
        fu->cancel();
    }

    return 0;
}

int _BroadcastCenter::processPostBack(BroadcastMessage msg) {
    if(msg->isComplete()) {
        mPostBackCompleted = true;
    }

    dispatchMessage(nullptr,msg->getData());
    return 0;
}

int _BroadcastCenter::processSubscribePersistence(BroadcastMessage msg) {
    Inspect(msg->mSocket == nullptr,-1);

    AutoLock l(mPersistWLock);
    mPersistenceClient = msg->mSocket;
    return 0;
}

int _BroadcastCenter::processSubscribeDLQ(BroadcastMessage msg) {
    AutoLock l(mDlqMutex);
    mDlqClient = msg->mSocket;
    return 0;
}
    

int _BroadcastCenter::registWaitAckTask(BroadcastMessage msg) {
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

int _BroadcastCenter::processPublish(BroadcastMessage msg) {
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
                    int random = st(System)::CurrentTimeMillis()%channels->size();
                    auto stream = channels->get(random);
                    if(originStream == stream) {
                        if(channels->size() > 1) {
                            (random >= channels->size() -1)?random = 0:random++;
                            stream = channels->get(random);
                        }
                    }

                    if(stream == originStream || stream->write(packet) < 0) {
                        BroadcastDLQMessage dlqMessage = BroadcastDLQMessage::New();
                        dlqMessage->setCode(stream == originStream?st(BroadcastDLQMessage)::NoClient:st(BroadcastDLQMessage)::ClientDisconnect)
                                  ->setData(packet)
                                  ->setToken(msg->getToken())
                                  ->setPointTime(st(System)::CurrentTimeMillis())
                                  ->setSrcAddress(msg->mSocket->getInetAddress()->getAddress());
                        processSendFailMessage(dlqMessage);
                    }
                } else {
                    auto ll = ArrayList<OutputStream>::New();
                    bool isFirst = true;
                    ForEveryOne(stream,channels) {
                        if(stream != originStream && stream->write(packet) < 0) {
                            ll->add(stream);
                            BroadcastDLQMessage dlqMessage = BroadcastDLQMessage::New();
                            auto s = Cast<SocketOutputStream>(stream);
                            dlqMessage->setCode(st(BroadcastDLQMessage)::ClientDisconnect)
                                    ->setData(isFirst?packet:nullptr)
                                    ->setSrcAddress(msg->mSocket->getInetAddress()->getAddress())
                                    ->setDestAddress(s->getSocket()->getInetAddress()->getAddress())
                                    ->setPointTime(st(System)::CurrentTimeMillis())
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
                BroadcastDLQMessage dlqMessage = BroadcastDLQMessage::New();
                dlqMessage->setCode(st(BroadcastDLQMessage)::NoClient)
                            ->setData(mConverter->generatePacket(msg))
                            ->setPointTime(st(System)::CurrentTimeMillis())
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
                stickyMap = HashMap<String,ByteArray>::New();
                mStickyMessages->put(channel,stickyMap);
            }
            stickyMap->put(msg->getToken(),mConverter->generatePacket(msg));
        });
    }

    return 0;
}

int _BroadcastCenter::processSubscribe(BroadcastMessage msg) {
    Inspect(msg->mSocket == nullptr,-1);

    mChannelGroups->syncWriteAction([this,&msg]{
        auto channel = msg->getChannel();
        auto channels = mChannelGroups->get(channel);
        if(channels == nullptr) {
            channels = ArrayList<OutputStream>::New();
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

int _BroadcastCenter::processUnSubscribe(BroadcastMessage msg) {
    Inspect(msg->mSocket == nullptr,-1);

    mChannelGroups->syncWriteAction([this,&msg]{
        auto channel = msg->getChannel();
        auto channels = mChannelGroups->get(channel);
        if(channels != nullptr && channels->size() > 0) {
            BroadcastMessage resp = BroadcastMessage::New(channel,nullptr,st(BroadcastMessage)::Detach);
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

bool _BroadcastCenter::processSendFailMessage(BroadcastDLQMessage msg) {
    String token = msg->getToken();
    Synchronized(mDlqMutex){
        Inspect(mDlqClient == nullptr,false);
        BroadcastMessage resp = BroadcastMessage::New(nullptr,msg->serialize(),st(BroadcastMessage)::Publish);
        return mDlqClient->getOutputStream()->write(mConverter->generatePacket(resp)) > 0;
    }

    return false;
}

_BroadcastCenter::~_BroadcastCenter() {
    this->close();
}

}
