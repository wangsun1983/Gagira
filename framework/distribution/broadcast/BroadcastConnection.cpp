#include <mutex>

#include "BroadcastConnection.hpp"
#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "BroadcastMessage.hpp"
#include "InitializeException.hpp"
#include "ForEveryOne.hpp"
#include "Inspect.hpp"
#include "BroadcastSustainMessage.hpp"

using namespace obotcha;

namespace gagira {

//-------- BroadcastMessageParam 
_BroadcastMessageParam::_BroadcastMessageParam() {
    mFlags = 0;
    mTTL = 0;
    mDelayInterval = 0;
}

_BroadcastMessageParam* _BroadcastMessageParam::setFlags(uint32_t flags) {
    mFlags = flags;
    return this;
}

_BroadcastMessageParam* _BroadcastMessageParam::setTTL(uint32_t ttl) {
    mTTL = ttl;
    return this;
}

_BroadcastMessageParam* _BroadcastMessageParam::setDelayInterval(uint32_t delay) {
    mDelayInterval = delay;
    return this;
}


uint32_t _BroadcastMessageParam::getFlags() {
    return mFlags;
}

uint32_t _BroadcastMessageParam::getTTL() {
    return mTTL;
}

uint32_t _BroadcastMessageParam::getDelayInterval() {
    return mDelayInterval;
}

sp<_BroadcastMessageParam> _BroadcastMessageParam::build() {
    return AutoClone(this);
}

//---------- BroadcastConnection
_BroadcastConnection::_BroadcastConnection(String s,BroadcastConnectionListener l) {
    HttpUrl url = HttpUrl::New(s);
    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find BroadcastCenter");
    }

    mListener = l;
    mSocketMonitor = SocketMonitor::New();
    mParser = DistributeMessageParser::New(1024*4);
    mConverter = DistributeMessageConverter::New();

    mIsConnected = false;
    mStatusReadWriteLock = ReadWriteLock::New();
    mStatusWriteLock = mStatusReadWriteLock->getWriteLock();
    mStatusReadLock = mStatusReadWriteLock->getReadLock();
}

int _BroadcastConnection::connect() {
    mSock = SocketBuilder::New()->setAddress(mAddress)->newSocket();
    Inspect(mSock->connect() < 0,-1);

    mInput = mSock->getInputStream();
    mOutput = mSock->getOutputStream();

    if(mListener != nullptr) {
        mListener->onConnect();
    }

    AutoLock l(mStatusWriteLock);
    mIsConnected = true;

    return mSocketMonitor->bind(mSock,AutoClone(this));
}

bool _BroadcastConnection::subscribeChannel(String channel) {
    BroadcastMessage msg = BroadcastMessage::New(channel,nullptr,st(BroadcastMessage)::Subscribe);
    return sendMessage(msg);
}

bool _BroadcastConnection::unSubscribeChannel(String channel) {
    BroadcastMessage msg = BroadcastMessage::New(channel,nullptr,st(BroadcastMessage)::UnSubscribe);
    return sendMessage(msg);
}

bool _BroadcastConnection::subscribePersistenceChannel() {
    BroadcastMessage msg = BroadcastMessage::New(nullptr,nullptr,st(BroadcastMessage)::SubscribePersistence);
    return sendMessage(msg);
}

bool _BroadcastConnection::subscribeDLQChannel() {
    BroadcastMessage msg = BroadcastMessage::New(nullptr,nullptr,st(BroadcastMessage)::SubscribeDLQ);
    return sendMessage(msg);
}

bool _BroadcastConnection::postBackMessage(ByteArray data,uint32_t flags) {
    BroadcastMessage msg = BroadcastMessage::New(nullptr,data,flags|st(BroadcastMessage)::PostBack);
    return sendMessage(msg);
}

void _BroadcastConnection::onSocketMessage(st(Net)::Event event,Socket s,ByteArray data) {
    switch(event) {
        case st(Net)::Event::Message: {
            mParser->pushData(data);
            auto result = mParser->doParse();
            if(result != nullptr && result->size() != 0) {
                ForEveryOne(data,result) {
                    auto msg = mConverter->generateMessage<BroadcastMessage>(data);
                    String channel = msg->getChannel();
                    if(mListener != nullptr) {
                        if(msg->getType() == st(BroadcastMessage)::Detach) {
                            mListener->onDetach(channel);
                        } else {
                            if(msg->getType() != st(BroadcastMessage)::Sustain) {
                                int ret = mListener->onMessage(channel,msg->getData());
                                if(msg->isAcknowledge() && ret == st(BroadcastConnectionListener)::AutoAck) {
                                    msg->setFlags(st(BroadcastMessage)::Ack);
                                    msg->clearData();
                                    s->getOutputStream()->write(mConverter->generatePacket(msg));
                                }
                            } else {
                                BroadcastSustainMessage sustainMsg = BroadcastSustainMessage::New();
                                sustainMsg->deserialize(msg->getData());
                                mListener->onSustain(sustainMsg->getCode(),sustainMsg->getMessage());
                            }
                        }
                    }
                }
            }
        }

        case st(Net)::Event::Connect:{
            //call onConnect when connected server successfully
            break;
        }

        case st(Net)::Event::Disconnect:{
            if(mListener != nullptr) {
                mListener->onDisconnect();
            }
            close();
            break;
        }

        default:
        break;
    }
}

int _BroadcastConnection::close() {
    AutoLock l(mStatusWriteLock);
    mIsConnected = false;
            
    if(mSock != nullptr) {
        mSocketMonitor->unbind(mSock,false);
        mSock->close();
        mSock = nullptr;
    }
    
    if(mOutput != nullptr) {
        mOutput->close();
        mOutput = nullptr;
    }
    
    if(mInput != nullptr) {
        mInput->close();
        mInput = nullptr;
    }
    return 0;
}

bool _BroadcastConnection::sendMessage(BroadcastMessage msg) {
    AutoLock l(mStatusReadLock);
    Inspect(!mIsConnected,false);
    auto ret = mOutput->write(mConverter->generatePacket(msg)) > 0;
    return ret;
}

_BroadcastConnection::~_BroadcastConnection() {
    close();
    if(mSocketMonitor != nullptr) {
        mSocketMonitor->close();
        mSocketMonitor = nullptr;
    }
}



}
