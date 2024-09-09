#include <mutex>

#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "QueueConnection.hpp"
#include "QueueMessage.hpp"
#include "NetEvent.hpp"
#include "InitializeException.hpp"
#include "ForEveryOne.hpp"
#include "Inspect.hpp"

using namespace obotcha;

namespace gagira {

//----------
_QueueWaitResult::_QueueWaitResult() {
    mMutex = Mutex::New();
    mCond = Condition::New();
    mProcessed = false;
}

void _QueueWaitResult::setResponse(MessageResponse resp) {
    AutoLock l(mMutex);
    mProcessed = true;
    mResponse = resp;
}

MessageResponse _QueueWaitResult::getResponse() {
    return mResponse;
}

int _QueueWaitResult::wait(int interval) {
    AutoLock l(mMutex);
    if(!mProcessed) {
        return mCond->wait(mMutex,interval);
    }

    return 0;
}

void _QueueWaitResult::setProcessed() {
    AutoLock l(mMutex);
    mProcessed = true;
}

bool _QueueWaitResult::isProcessed() {
    AutoLock l(mMutex);
    return mProcessed;
}

void _QueueWaitResult::notify() {
    AutoLock l(mMutex);
    mCond->notify();
}

//---------- QueueConnection
_QueueConnection::_QueueConnection(String s) {
    HttpUrl url = HttpUrl::New(s);
    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find MqCenter");
    }
    mParser = DistributeMessageParser::New(1024*4);
    mConverter = DistributeMessageConverter::New();
    mSocketMonitor = SocketMonitor::New(1);
    mBuilder = QueueMessageBuilder::New();
    mWaitResults = HashMap<Uint32,QueueWaitResult>::New();
    mMutex = Mutex::New();
}

int _QueueConnection::connect() {
    mSock = SocketBuilder::New()->setAddress(mAddress)->newSocket();
    Inspect(mSock->connect() < 0,-1);

    mInput = mSock->getInputStream();
    mOutput = mSock->getOutputStream();
    return mSocketMonitor->bind(mSock,AutoClone(this));
}

void _QueueConnection::onSocketMessage(st(Net)::Event event,Socket socket,ByteArray data) {
    switch(event) {
        case st(Net)::Event::Message: {
            mParser->pushData(data);
            auto result = mParser->doParse();
            if(result != nullptr && result->size() != 0) {
                ForEveryOne(record,result) {
                    auto response = mConverter->generateMessage<MessageResponse>(record);
                    QueueWaitResult waiter = nullptr;
                    {
                        AutoLock l(mMutex);
                        waiter = mWaitResults->get(Uint32::New(response->getReqId()));
                    }
                    waiter->setResponse(response);
                    waiter->notify();
                }
            }
        } break;

        case st(Net)::Event::Disconnect: {
            notifyAllWaiters(ENETUNREACH);
        } break;
    }
}

void _QueueConnection::notifyAllWaiters(uint32_t reason) {
    AutoLock l(mMutex);
    auto response = MessageResponse::New();
    response->setResult(reason);
    ForEveryOne(pair,mWaitResults) {
        auto waiter = pair->getValue();
        if(!waiter->isProcessed()) {
            waiter->setResponse(response);
            waiter->notify();
        }
    }
}

int _QueueConnection::close() {
    //send exit info to server
    notifyAllWaiters(EPIPE);
    {
        AutoLock l(mMutex);
        if(mSock != nullptr) {
            mSocketMonitor->close();
            mSocketMonitor->waitForExit();
            if(mSock != nullptr) {
                mSock->close();
                mSock = nullptr;
            }
            if(mOutput != nullptr) {
                mOutput->close();
                //mOutput = nullptr;
            }
            if(mInput != nullptr) {
                mInput->close();
                //mInput = nullptr;
            }
        }
    }
    return 0;
}

_QueueConnection::~_QueueConnection() {
    close();
}

MessageResponse _QueueConnection::communicate(QueueMessage msg,int interval) {
    QueueWaitResult waitRespResult = QueueWaitResult::New();
    auto id = Uint32::New(msg->getReqId());
    {
        AutoLock l(mMutex);
        mWaitResults->put(id,waitRespResult);
    }
    
    if(mOutput->write(mConverter->generatePacket(msg)) <= 0) {
        AutoLock l(mMutex);
        mWaitResults->remove(id);
        return MessageResponse::New(msg->getReqId(),ENETUNREACH);
    }
    if(waitRespResult->wait(interval) != 0) {
        return MessageResponse::New(msg->getReqId(),ETIMEDOUT);
    } else {
        AutoLock l(mMutex);
        auto waiter = mWaitResults->remove(id);
        return waiter->getResponse();
    }
}

void _QueueConnection::sendAck(uint32_t reqid) {
    auto ack_msg = mBuilder->setAsAck()->setAckReqId(reqid)->build();
    mOutput->write(mConverter->generatePacket(ack_msg));
}

}
