#include "QueueCenter.hpp"
#include "QueueMessage.hpp"
#include "SocketBuilder.hpp"
#include "HttpUrl.hpp"
#include "Inspect.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

_QueueCenter::_QueueCenter(String url,DistributeOption option):_DistributeCenter(url,option) {
    mConverter = createDistributeMessageConverter();
    mClients = createBlockingLinkedList<DistributeLinker>();
    mTasks = createBlockingLinkedList<QueueMessage>();
    isRunning = 1;
}

int _QueueCenter::start() {
    st(DistributeCenter)::start();
    st(Thread)::start();
    return 0;
}

int _QueueCenter::close() {
    st(DistributeCenter)::close();
    isRunning = 0;
    mClients->clear();
    mTasks->clear();
    return 0;
}

void _QueueCenter::run() {
    while(isRunning) {
        auto task = mTasks->takeFirst();
        while(isRunning) {
            auto client = mClients->takeFirst();
            if(client == nullptr || client->getSocket()->getOutputStream()->write(mConverter->generatePacket(task)) <= 0) {
                continue;
            }
            break;
        }
    }
}

_QueueCenter::~_QueueCenter() {

}

int _QueueCenter::onMessage(DistributeLinker linker,ByteArray data) {
    auto msg = mConverter->generateMessage<QueueMessage>(data);
    switch(msg->event) {
        case st(QueueMessage)::Acquire:
            mClients->putLast(linker);
        break;

        case st(QueueMessage)::Submit:
            msg->event = st(QueueMessage)::Task;
            mTasks->putLast(msg);
        break;

        case st(QueueMessage)::ClientNoWait:
            mClients->remove(linker);
        break;
    }

    return 0;
}

int _QueueCenter::onNewClient(DistributeLinker linker) {
    return 0;
}

int _QueueCenter::onDisconnectClient(DistributeLinker linker) {
    //TODO
    return 0;
}

}
