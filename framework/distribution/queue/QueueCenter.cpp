#include "QueueCenter.hpp"
#include "QueueMessage.hpp"
#include "SocketBuilder.hpp"
#include "HttpUrl.hpp"
#include "Inspect.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

//----- QueueWaitReceiver -----
_QueueWaitReceiver::_QueueWaitReceiver(uint32_t reqid,DistributeLinker linker) {
    this->req_id = reqid;
    this->linker = linker;
}


//----- QueueCenter -----
_QueueCenter::_QueueCenter(String url,DistributeOption option):_DistributeCenter(url,option) {
    mConverter = DistributeMessageConverter::New();
    mMutex = Mutex::New();
    mWaiters = LinkedList<QueueWaitReceiver>::New();
    mTasks = LinkedList<QueueMessage>::New();
    mProcessingMessages = HashMap<DistributeLinker,HashMap<Uint32,QueueMessage>>::New();

    if(option != nullptr) {
        mHandler = option->getHandler();
    }

    isRunning = 1;
}

int _QueueCenter::start() {
    st(DistributeCenter)::start();
    return 0;
}

int _QueueCenter::close() {
    st(DistributeCenter)::close();

    AutoLock l(mMutex);
    Inspect(isRunning == 0,0)
    isRunning = 0;
    mWaiters->clear();
    mTasks->clear();
    return 0;
}

_QueueCenter::~_QueueCenter() {

}

bool _QueueCenter::handlePermission(DistributeLinker linker,QueueMessage msg) {
    if(msg->getEvent() != st(QueueMessage)::AckFromClient 
        && mHandler != nullptr 
        && mHandler->onRequest(linker,msg) != 0) {
        auto response = MessageResponse::New(msg->getReqId(),0);
        response->setResult(EPERM);
        linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        return false;
    }

    return true;
}

int _QueueCenter::onMessage(DistributeLinker linker,ByteArray data) {
    auto msg = mConverter->generateMessage<QueueMessage>(data);
    
    if(!handlePermission(linker,msg)) {
        return 0;
    }

    switch(msg->getEvent()) {
        case st(QueueMessage)::Acquire: {
            AutoLock l(mMutex);
            if(mTasks->size() != 0) {
                auto response = MessageResponse::New(msg->getReqId(),0);
                auto task = mTasks->takeFirst();
                task->setReqId(msg->getReqId());
                addProcessingMessageLocked(linker,task);
                response->setData(task->serialize());
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
            } else {
                auto waiter = QueueWaitReceiver::New(msg->getReqId(),linker);
                mWaiters->putLast(waiter);
            }
        } break;

        case st(QueueMessage)::Submit: {
            auto response = MessageResponse::New(msg->getReqId(),0);
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
            AutoLock l(mMutex);
            if(mWaiters->size() != 0) {
                msg->setEvent(st(QueueMessage)::Task);
                auto client = mWaiters->takeFirst();
                addProcessingMessageLocked(client->linker,msg);
                auto sendToClientData = MessageResponse::New(client->req_id,0);
                sendToClientData->setData(msg->serialize());
                //send to waiting client.
                if(TestDelayProcessInterval != 0) {
                    mMutex->unlock();
                    usleep(1000 * TestDelayProcessInterval);
                }
                client->linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(sendToClientData));
            } else {
                mTasks->putLast(msg);
            }
        } break;

        case st(QueueMessage)::Cancel: {
            auto response = MessageResponse::New(msg->getReqId(),0);
            AutoLock l(mMutex);
            auto cancel_id = msg->getCancelId();
            auto wait_msg = getProcessingMessageLocked(linker,cancel_id);
            if(wait_msg != nullptr) {
                response->setResult(EAGAIN);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                return 0;
            } else {
                auto iterator = mWaiters->getIterator();            
                for(;iterator->hasValue();iterator->next()) {
                    auto waiter = iterator->getValue();
                    if(waiter->req_id == cancel_id) {
                        iterator->remove();
                        response->setResult(0);
                        linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                        return 0;
                    }
                }
            }
            //if no wait message cancel
            response->setResult(EINVAL);
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(QueueMessage)::AckFromClient: {
            AutoLock l(mMutex);
            removeProcessingMessageLocked(linker,msg->getReqId());
        } break;
    }

    return 0;
}

int _QueueCenter::onNewClient(DistributeLinker linker) {
    return 0;
}

int _QueueCenter::onDisconnectClient(DistributeLinker linker) {
    AutoLock l(mMutex);
    auto req_map = mProcessingMessages->remove(linker);
    if(req_map != nullptr) {
        //TODO shall we inset task in sequence???
        ForEveryOne(pair,req_map) {
            mTasks->putFirst(pair->getValue());
        }
    }
    return 0;
}

void _QueueCenter::addProcessingMessageLocked(DistributeLinker linker,QueueMessage msg) {
    auto reqMap = mProcessingMessages->get(linker);
    if(reqMap == nullptr) {
        reqMap = HashMap<Uint32,QueueMessage>::New();
        mProcessingMessages->put(linker,reqMap);
    }

    reqMap->put(Uint32::New(msg->getReqId()),msg);
}

void _QueueCenter::removeProcessingMessageLocked(DistributeLinker linker,uint32_t req_id) {
    auto reqMap = mProcessingMessages->get(linker);
    if(reqMap != nullptr) {
        reqMap->remove(Uint32::New(req_id));
    }
}

QueueMessage _QueueCenter::getProcessingMessageLocked(DistributeLinker linker,uint32_t req_id) {
    auto reqMap = mProcessingMessages->get(linker);
    if(reqMap != nullptr) {
        return reqMap->get(Uint32::New(req_id));
    }

    return nullptr;
}

IMPL_UNIT_TEST_INTERFACE(
    uint64_t _QueueCenter::getRemainTaskSize() {
        AutoLock l(mMutex);
        return mTasks->size();
    }

    uint64_t _QueueCenter::getRemainWaiterSize() {
        AutoLock l(mMutex);
        return mWaiters->size();
    }

    void _QueueCenter::setDelayProcessInterval(uint32_t interval) {
        TestDelayProcessInterval = interval;
    }
)

}
