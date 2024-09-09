#include <mutex>

#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "MapConnection.hpp"
#include "MapMessage.hpp"
#include "NetEvent.hpp"
#include "InitializeException.hpp"
#include "ForEveryOne.hpp"
#include "Inspect.hpp"

using namespace obotcha;

namespace gagira {

//---------- MapResultWaiter --------
_MapResultWaiter::_MapResultWaiter() {
    mMutex = Mutex::New();
    mCond = Condition::New();
    isProcessed = false;
}

void _MapResultWaiter::wait() {
    AutoLock l(mMutex);
    if(!isProcessed) {
        mCond->wait(mMutex);
    }
}

void _MapResultWaiter::setResponse(MapMessage response) {
    AutoLock l(mMutex);
    isProcessed = true;
    this->mResponse = response;
    mCond->notifyAll();
}

MapMessage _MapResultWaiter::getResponse() {
    AutoLock l(mMutex);
    return mResponse;
}

//---------- MapResultWaiterManager ----------
_MapResultWaiterManager::_MapResultWaiterManager() {
    mWaiters = ConcurrentHashMap<uint64_t,MapResultWaiter>::New();
}

void _MapResultWaiterManager::notifyResult(MapMessage resp) {
    auto waiter = mWaiters->remove(resp->getId());
    if(waiter != nullptr) {
        waiter->setResponse(resp);
    }
}

void _MapResultWaiterManager::addWaiter(uint64_t id,MapResultWaiter waiter) {
    mWaiters->put(id,waiter);
}

void _MapResultWaiterManager::close() {
    MapMessageResponse response = MapMessageResponse::New(0,st(MapMessage)::Type::Disconnect,0);
    ForEveryOne(pair,mWaiters) {
        auto waiter = pair->getValue();
        waiter->setResponse(response);
    }

    mWaiters->clear();
}

//---------- MapConnection ----------
_MapConnection::_MapConnection(String s,MapConnectionListener l) {
    mReqId = 1;
    mMutex = Mutex::New();
    mError = 0;
    mWaiterManager = MapResultWaiterManager::New();

    HttpUrl url = HttpUrl::New(s);
    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find MqCenter");
    }
    
    mListener = l;
    mSocketMonitor = SocketMonitor::New(1);
    mParser = DistributeMessageParser::New(1024*4);
    mConverter = DistributeMessageConverter::New();
}

int _MapConnection::connect() {
    mError = 0;
    mSock = SocketBuilder::New()->setAddress(mAddress)->newSocket();
    Inspect(mSock->connect() < 0,-errno);

    mInput = mSock->getInputStream();
    mOutput = mSock->getOutputStream();

    if(mListener != nullptr) {
        mListener->onConnect();
    }

    mError = mSocketMonitor->bind(mSock,AutoClone(this));
    return mError;
}

void _MapConnection::onSocketMessage(st(Net)::Event event,Socket s,ByteArray data) {
    switch(event) {
        case st(Net)::Event::Message: {
            mParser->pushData(data);
            auto result = mParser->doParse();
            if(result != nullptr && result->size() != 0) {
                ForEveryOne(data,result) {
                    auto msg = mConverter->generateMessage<MapMessage>(data);
                    switch(msg->getEvent()) {
                        case st(MapMessage)::NotifyUpdate:
                            if(mListener != nullptr) {
                                mListener->onEvent(msg->getResult(),msg->getTag(),msg->getData());
                            }
                            break;

                        case st(MapMessage)::NotifyRemove:
                            if(mListener != nullptr) {
                                mListener->onEvent(msg->getResult(),msg->getTag(),msg->getData());
                            }
                            break;
                        
                        case st(MapMessage)::ResponseMonitor:
                        case st(MapMessage)::ResponseUnMonitor:
                        case st(MapMessage)::ResponseUpdate:
                        case st(MapMessage)::ResponseAcquire:
                        case st(MapMessage)::ResponseRemove:
                            mWaiterManager->notifyResult(msg);
                    }
                }
            }
        }

        case st(Net)::Event::Connect:{
            break;
        }

        case st(Net)::Event::Disconnect:{
            //info client
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

int _MapConnection::close() {
    AutoLock l(mMutex);
    mError = 0;

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

    //notify all waiters
    mWaiterManager->close();
    return 0;
}

int _MapConnection::monitor(ArrayList<String> list) {
    auto id = getReqId();
    auto message = MapMonitorMessage::New(id,list);
    auto data = mConverter->generatePacket(message);
    return waitForResult(id,data);
}

int _MapConnection::monitor(String tag) {
    auto id = getReqId();
    auto message = MapMonitorMessage::New(id,tag);
    auto data = mConverter->generatePacket(message);
    return waitForResult(id,data);
}

int _MapConnection::unMonitor(ArrayList<String> list) {
    auto id = getReqId();
    auto message = MapUnMonitorMessage::New(id,list);
    auto data = mConverter->generatePacket(message);
    return waitForResult(id,data);
}

int _MapConnection::unMonitor(String tag) {
    auto id = getReqId();
    auto message = MapUnMonitorMessage::New(id,tag);
    auto data = mConverter->generatePacket(message);
    return waitForResult(id,data);
}

int _MapConnection::remove(String tag) {
    auto id = getReqId();
    auto message = MapRemoveMessage::New(id,tag,nullptr);
    auto data = mConverter->generatePacket(message);
    return waitForResult(id,data);
}

int _MapConnection::waitForResult(uint64_t id,ByteArray data) {
    mError = 0;

    auto waiter = MapResultWaiter::New();
    mWaiterManager->addWaiter(id,waiter);
    {
        AutoLock l(mMutex);
        if(mOutput != nullptr) {
            if(mOutput->write(data) == 0) {
                mError = -ENETUNREACH;
                return mError;
            }
        } else {
            mError = -ESHUTDOWN ;
            return mError;
        }
    }

    waiter->wait();
    mError = -waiter->getResponse()->getResult();   
    return mError;
}

int _MapConnection::getReqId() {
    AutoLock l(mMutex);
    return mReqId++;
}

int _MapConnection::getErr() {
    return mError;
}

_MapConnection::~_MapConnection() {
    close();
    if(mSocketMonitor != nullptr) {
        mSocketMonitor->close();
        mSocketMonitor = nullptr;
    }
}

}
