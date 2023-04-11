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

//---------- SpaceConnection
_QueueConnection::_QueueConnection(String s) {
    HttpUrl url = createHttpUrl(s);
    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find MqCenter");
    }
    mParser = createDistributeMessageParser(1024*4);
    mConverter = createDistributeMessageConverter();
    mSocketMonitor = createSocketMonitor(1);
    mTasks = createBlockingLinkedList<ByteArray>();
}

int _QueueConnection::connect() {
    mSock = createSocketBuilder()->setAddress(mAddress)->newSocket();
    Inspect(mSock->connect() < 0,-1);

    mInput = mSock->getInputStream();
    mOutput = mSock->getOutputStream();
    return mSocketMonitor->bind(mSock,AutoClone(this));
}

void _QueueConnection::onSocketMessage(int event,Socket socket,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Message: {
            mParser->pushData(data);
            auto result = mParser->doParse();
            if(result != nullptr && result->size() != 0) {
                ForEveryOne(data,result) {
                    auto msg = mConverter->generateMessage<QueueMessage>(data);
                    if(msg->event == st(QueueMessage)::Task) {
                        mTasks->putLast(msg->data);
                    }
                }
            }
        }
    }
}

int _QueueConnection::close() {
    if(mSock != nullptr) {
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

_QueueConnection::~_QueueConnection() {
    close();
}

}
