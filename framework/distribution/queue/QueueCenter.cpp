#include "QueueCenter.hpp"
#include "QueueMessage.hpp"
#include "SocketBuilder.hpp"
#include "HttpUrl.hpp"
#include "Inspect.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

//----- QueueWaitReceiver -----
_QueueWaitReceiver::_QueueWaitReceiver(uint32_t reqid,DistributeLinker linker) {
    req_id = reqid;
    this->linker = linker;
}


//----- QueueCenter -----
_QueueCenter::_QueueCenter(String url,DistributeOption option):_DistributeCenter(url,option) {
    mConverter = DistributeMessageConverter::New();
    mMutex = Mutex::New();
    mWaiters = LinkedList<QueueWaitReceiver>::New();
    mTasks = LinkedList<QueueMessage>::New();
    mResponseWaiters = HashMap<DistributeLinker,HashMap<Uint32,QueueMessage>>::New();
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
    AutoLock l(mMutex);
    st(DistributeCenter)::close();
    isRunning = 0;
    mWaiters->clear();
    mTasks->clear();
    return 0;
}

_QueueCenter::~_QueueCenter() {

}

int _QueueCenter::onMessage(DistributeLinker linker,ByteArray data) {
    auto msg = mConverter->generateMessage<QueueMessage>(data);
    
    bool permit = true;
    if(mHandler != nullptr) {
        auto ret = mHandler->onRequest(linker,msg);
        permit = (ret == 0);
    }

    switch(msg->getEvent()) {
        case st(QueueMessage)::Acquire: {
            auto response = MessageResponse::New(msg->getReqId(),0);
            if(!permit) {
                response->setResult(EPERM);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                return 0;
            }
            
            AutoLock l(mMutex);
            if(mTasks->size() != 0) {
                auto task = mTasks->takeFirst();
                task->setReqId(msg->getReqId());
                addResponseWaitersLocked(linker,task);

                response->setData(task->serialize());
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
            } else {
                auto waiter = QueueWaitReceiver::New(msg->getReqId(),linker);
                mWaiters->putLast(waiter);
            }
        } break;

        case st(QueueMessage)::Submit: {
            auto response = MessageResponse::New(msg->getReqId(),0);
            if(!permit) {
                response->setResult(EPERM);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
            } else {
                msg->setEvent(st(QueueMessage)::Task);
                AutoLock l(mMutex);
                if(mWaiters->size() != 0) {
                    auto client = mWaiters->takeFirst();
                    addResponseWaitersLocked(client->linker,msg);
                    msg->setReqId(client->req_id);
                    auto sendToClientData = MessageResponse::New(msg->getReqId(),0);
                    sendToClientData->setData(msg->serialize());
                    //send to client
                    client->linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(sendToClientData));
                } else {
                    mTasks->putLast(msg);
                }
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(QueueMessage)::Cancel: {
            auto response = MessageResponse::New(msg->getReqId(),0);
            if(!permit) {
                response->setResult(EPERM);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                return 0;
            }

            AutoLock l(mMutex);
            auto cancel_id = msg->getCancelId();
            auto iterator = mWaiters->getIterator();            
            for(;iterator->hasValue();iterator->next()) {
                auto waiter = iterator->getValue();
                if(waiter->req_id == cancel_id) {
                    iterator->remove();
                    linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                    return 0;
                }
            }

            auto wait_msg = getResponseWaitersLocked(linker,cancel_id);
            if(wait_msg != nullptr) {
                response->setResult(EAGAIN);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                return 0;
            }

            //if no wait message cancel
            response->setResult(EINVAL);
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(QueueMessage)::AckFromClient: {
            AutoLock l(mMutex);
            removeResponseWaiterLocked(linker,msg->getReqId());
        } break;
    }

    return 0;
}

int _QueueCenter::onNewClient(DistributeLinker linker) {
    return 0;
}

int _QueueCenter::onDisconnectClient(DistributeLinker linker) {
    AutoLock l(mMutex);
    auto req_map = mResponseWaiters->get(linker);
    if(req_map != nullptr) {
        //TODO shall we inset task in sequence???
        ForEveryOne(pair,req_map) {
            mTasks->putFirst(pair->getValue());
        }
    }
    return 0;
}

void _QueueCenter::addResponseWaitersLocked(DistributeLinker linker,QueueMessage msg) {
    auto reqMap = mResponseWaiters->get(linker);
    if(reqMap == nullptr) {
        reqMap = HashMap<Uint32,QueueMessage>::New();
        mResponseWaiters->put(linker,reqMap);
    }

    reqMap->put(Uint32::New(msg->getReqId()),msg);
}

void _QueueCenter::removeResponseWaiterLocked(DistributeLinker linker,uint32_t req_id) {
    auto reqMap = mResponseWaiters->get(linker);
    if(reqMap != nullptr) {
        reqMap->remove(Uint32::New(req_id));
    }
}

QueueMessage _QueueCenter::getResponseWaitersLocked(DistributeLinker linker,uint32_t req_id) {
    auto reqMap = mResponseWaiters->get(linker);
    if(reqMap != nullptr) {
        return reqMap->get(Uint32::New(req_id));
    }

    return nullptr;
}

}
