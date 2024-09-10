#ifndef __GAGRIA_QUEUE_CONNECTION_HPP__
#define __GAGRIA_QUEUE_CONNECTION_HPP__

#include "String.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "SocketMonitor.hpp"
#include "Mutex.hpp"
#include "DistributeMessageParser.hpp"
#include "DistributeMessageConverter.hpp"
#include "SocketBuilder.hpp"
#include "BlockingLinkedList.hpp"
#include "QueueMessage.hpp"
#include "QueueMessageBuilder.hpp"
#include "Mutex.hpp"
#include "Condition.hpp"
#include "Process.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(QueueWaitResult) {
public:
    _QueueWaitResult();
    void setResponse(MessageResponse);
    MessageResponse getResponse();
    int wait(int interval = 0);
    void notify();
    void setProcessed();
    bool isProcessed();
private:
    Mutex mMutex;
    Condition mCond;
    MessageResponse mResponse;
    bool mProcessed;
};

DECLARE_TEMPLATE_CLASS(QueueData,T) {
public:
    T data;
    int err;
};

DECLARE_CLASS(QueueConnection) IMPLEMENTS(SocketListener) {
public:
    _QueueConnection(String);
    ~_QueueConnection();

    template<typename T>
    int add(T task) {
        auto msg = mBuilder->setData(task->serialize())
                          ->setAsSubmit()
                          ->build();
        auto response = communicate(msg,0);
        return -response->getResult();
    }

    template<typename T>
    QueueData<T> get(int interval = 0) {
        QueueData<T> result = QueueData<T>::New();
        result->data = nullptr;
        result->err = 0;
        
        auto msg = mBuilder->setAsRequire()->build();
        auto response = communicate(msg,interval);
        if(response->getResult() == ETIMEDOUT) {
            //send cancel messsage
            auto cancel_msg = mBuilder->setAsCancel()->setCancelId(msg->getReqId())->build();
            auto response = communicate(cancel_msg,0);
            if(response->getResult() == EAGAIN) {
                QueueWaitResult result_waiter = nullptr;
                {
                    AutoLock l(mMutex);
                    result_waiter = mWaitResults->get(Uint32::New(msg->getReqId()));
                }

                result_waiter->wait();
                auto again_response = result_waiter->getResponse();
                result->data = unpack<T>(again_response);;
                result->err = -response->getResult();
                if(result->err == 0) {
                    sendAck(msg->getReqId());
                }
                return result;
            } else {
                result->err = -ETIMEDOUT;
                return result;
            }
        }
        
        if(response->getData() != nullptr) {
            result->data = unpack<T>(response);
        }
        result->err = -response->getResult();

        if(result->err == 0) {
            sendAck(msg->getReqId());
        }
        return result;
    }

    int connect();
    int close();

private:
    MessageResponse communicate(QueueMessage,int interval = 0);
    void sendAck(uint32_t reqid);
    void notifyAllWaiters(uint32_t reason);

    template<typename T> 
    T unpack(MessageResponse response) {
        QueueMessage msg = QueueMessage::New();
        msg->deserialize(response->getData());
        T val = AutoCreate<T>();
        val->deserialize(msg->getData());
        return val;
    }

    InetAddress mAddress;
    Socket mSock;
    InputStream mInput;
    OutputStream mOutput;
    DistributeMessageConverter mConverter;
    DistributeMessageParser mParser;
    SocketMonitor mSocketMonitor;
    //BlockingLinkedList<ByteArray> mTasks;
    Mutex mMutex;
    HashMap<Uint32,QueueWaitResult> mWaitResults;

    QueueMessageBuilder mBuilder;

    void onSocketMessage(st(Net)::Event,Socket,ByteArray);
    
};

}
#endif
