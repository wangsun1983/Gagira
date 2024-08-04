#ifndef __GAGRIA_QUEUE_CONNECTION_HPP__
#define __GAGRIA_QUEUE_CONNECTION_HPP__

#include "String.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "SocketMonitor.hpp"
#include "Mutex.hpp"
#include "ByteRingArray.hpp"
#include "ByteRingArrayReader.hpp"
#include "DistributeMessageParser.hpp"
#include "DistributeMessageConverter.hpp"
#include "SpaceMessage.hpp"
#include "ReadWriteLock.hpp"
#include "System.hpp"
#include "SocketBuilder.hpp"
#include "BlockingLinkedList.hpp"
#include "QueueMessage.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(QueueConnection) IMPLEMENTS(SocketListener) {
public:
    _QueueConnection(String);
    ~_QueueConnection();

    template<typename T>
    int submitTask(T task) {
        QueueMessage msg = QueueMessage::New(st(QueueMessage)::Submit,task->serialize());
        return mOutput->write(mConverter->generatePacket(msg));
    }

    template<typename T>
    T acquireTask(int interval = 0) {
        QueueMessage msg = QueueMessage::New(st(QueueMessage)::Acquire,nullptr);
        if(mOutput == nullptr || mOutput->write(mConverter->generatePacket(msg)) < 0) {
            return nullptr;
        }
        auto result = AutoCreate<T>();
        auto data = mTasks->takeFirst(interval);
        if(data != nullptr) {
            result->deserialize(data);
            return result;
        }

        msg = QueueMessage::New(st(QueueMessage)::ClientNoWait,nullptr);
        mOutput->write(mConverter->generatePacket(msg));    
        return nullptr;
    }

    int connect();
    int close();

private:
    InetAddress mAddress;
    Socket mSock;
    InputStream mInput;
    OutputStream mOutput;
    DistributeMessageConverter mConverter;
    DistributeMessageParser mParser;
    SocketMonitor mSocketMonitor;
    BlockingLinkedList<ByteArray> mTasks;

    void onSocketMessage(int,Socket,ByteArray);
    
};

}
#endif
