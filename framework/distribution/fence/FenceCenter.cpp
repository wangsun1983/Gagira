#include "FenceCenter.hpp"
#include "FenceMessage.hpp"
#include "AutoLock.hpp"
#include "ExecutorBuilder.hpp"
#include "ForEveryOne.hpp"
#include "Log.hpp"
#include "ForEveryOne.hpp"
#include "OStdInstanceOf.hpp"

using namespace obotcha;

namespace gagira {

//FenceWaiter
_FenceWaiter::_FenceWaiter(String token,int flag) {
    this->waiterToken = token;
    this->flag = flag;
}

//FenceInfo
_FenceInfo::_FenceInfo(String name) {
    mutex = Mutex::New();
    fence = Fence::New(name);
    waiters = LinkedList<FenceWaiter>::New();
}

//FenceCenter
_FenceCenter::_FenceCenter(String url,FenceOption option):_DistributeCenter(url,option) {
    mFences = HashMap<String,FenceInfo>::New();
    mLinkers = HashMap<String,DistributeLinker>::New();
    mFenceOwners = HashMap<String,String>::New();
    mMutex = Mutex::New();
    mScheduledPool = ExecutorBuilder::New()->setMaxThreadNum(2)->newScheduledThreadPool();
    mHandler = option->getHandler();
}

_FenceCenter::~_FenceCenter() {
    //TODO
}

String _FenceCenter::genToken(DistributeLinker linker) {
    auto address = linker->getSocket()->getInetAddress();
    return address->getAddress()->append("::::",String::New(address->getPort()));
}

int _FenceCenter::onMessage(DistributeLinker linker,ByteArray data) {
    auto msg = mConverter->generateMessage<FenceMessage>(data);
    String linkerToken = genToken(linker);
    
    if(mHandler != nullptr) {
        int ret = mHandler->onRequest(linker,msg);
        if(ret < 0) {
            sendResponse(msg->getFenceName(),linker,-ret);
            return 0;
        }
    }
    
    printf("FenceCenter name is %s,event is %d \n",msg->getFenceName()->toChars(),msg->getEvent());
    switch(msg->getEvent()) {
        case st(FenceMessage)::ApplyReadFence: {
            AutoLock l(mMutex);
            mLinkers->put(linkerToken,linker);
            auto info = getIfEmptyCreate(msg);
            auto fence = Cast<ReadWriteFence>(info->fence);
            auto readfence = fence->getReadFence();
            auto writeFence = fence->getWriteFence();
            if(readfence->getCount() != 0 && writeFence->getCount() == 0) {
                readfence->addOwner(linker);
                sendResponse(msg->getFenceName(),linker);
            } else {
                //check write fence
                auto owner = writeFence->getOwner();
                if(owner != nullptr && !owner->equals(linkerToken)) {
                    info->waiters->putLast(FenceWaiter::New(linkerToken,st(FenceWaiter)::ReadFlag));
                    auto waittime = msg->getWaitTime();
                    if(waittime != 0) {
                        mScheduledPool->schedule(waittime,[&](String token,String name) {
                            AutoLock l(mMutex);
                            DistributeLinker linker = mLinkers->remove(token);
                            sendResponse(name,linker,ETIMEDOUT);
                        },linkerToken,fence->getName());
                    }
                } else {
                    readfence->addOwner(linker);
                    sendResponse(msg->getFenceName(),linker);
                }
            }
        } break;

        case st(FenceMessage)::ApplyReleaseReadFence: {
            sendResponse(msg->getFenceName(),linker,
                        -processReleaseReadOwner(msg->getFenceName(),linker));
        } break;

        case st(FenceMessage)::ApplyWriteFence: {
            AutoLock l(mMutex);
            mLinkers->put(linkerToken,linker);
            auto info = getIfEmptyCreate(msg);
            auto fence = Cast<ReadWriteFence>(info->fence);
            auto readfence = fence->getReadFence();
            auto writeFence = fence->getWriteFence();
            auto owner = writeFence->getOwner();

            if(readfence->getCount() != 0 && !linkerToken->equals(owner)) {
                info->waiters->putLast(FenceWaiter::New(linkerToken,st(FenceWaiter)::WriteFlag));
                auto waittime = msg->getWaitTime();
                if(waittime != 0) {
                    mScheduledPool->schedule(waittime,[&](String token,String name) {
                        AutoLock l(mMutex);
                        DistributeLinker myLinker = mLinkers->remove(token);
                        sendResponse(name,myLinker,ETIMEDOUT);
                    },linkerToken,fence->getName());
                }
            } else {
                if(owner != nullptr && !owner->equals(linkerToken)) {
                    info->waiters->putLast(FenceWaiter::New(linkerToken,st(FenceWaiter)::WriteFlag));
                } else {
                    writeFence->setOwner(linkerToken);
                    writeFence->incCount();
                    sendResponse(msg->getFenceName(),linker);
                }
            }
        } break;

        case st(FenceMessage)::ApplyReleaseWriteFence: {
            sendResponse(msg->getFenceName(),linker,
                        -processReleaseWriteOwner(msg->getFenceName(),linker));
        } break;

        case st(FenceMessage)::ApplyFence: {
            AutoLock l(mMutex);
            mLinkers->put(linkerToken,linker);
            printf("FenceCenter apply fence trace0,name is %s \n",msg->getFenceName()->toChars());
            auto info = getIfEmptyCreate(msg);
            auto fence = info->fence;
            auto owner = fence->getOwner();
            if(owner != nullptr && !owner->equals(linkerToken)) {
                 printf("FenceCenter apply fence trace1,name is %s \n",msg->getFenceName()->toChars());
                //add to wait list
                info->waiters->putLast(FenceWaiter::New(linkerToken));
                auto waittime = msg->getWaitTime();
                if(waittime != 0) {
                    mScheduledPool->schedule(waittime,[&](String token,String name) {
                        AutoLock l(mMutex);
                        DistributeLinker myLinker = mLinkers->remove(token);
                        sendResponse(name,myLinker,ETIMEDOUT);
                    },linkerToken,fence->getName());
                }
            } else {
                 printf("FenceCenter apply fence trace2,name is %s \n",msg->getFenceName()->toChars());
                //acquire lock success
                fence->setOwner(linkerToken);
                mFenceOwners->put(msg->getFenceName(),linkerToken);
                fence->incCount();
                sendResponse(msg->getFenceName(),linker);
            }
        } break;

        case st(FenceMessage)::ApplyReleaseFence: {
            sendResponse(msg->getFenceName(),
                         linker,
                         -processReleaseOwner(msg->getFenceName(),linkerToken));
        } break;
    }
    return -1;
}

int _FenceCenter::processReleaseOwner(String fencename,String linkertoken) {
    AutoLock l(mMutex);

    auto owner = mFenceOwners->get(fencename);
    if(owner == nullptr || !owner->equals(linkertoken)) {
        return -EINVAL;
    }
    
    auto info = mFences->get(fencename);
    if(info != nullptr) {
        info->fence->decCount();
        if(info->fence->isFree()) {
            AutoLock l(info->mutex);
            while(info->waiters->size() != 0) {
                auto nextWaiterToken = info->waiters->takeFirst()->waiterToken;
                auto nextWaiter = mLinkers->get(nextWaiterToken);
                if(nextWaiter == nullptr || sendResponse(fencename,nextWaiter) < 0) {
                    continue;
                }

                mFenceOwners->put(fencename,genToken(nextWaiter));
                break;
            }

            if(info->waiters->size() == 0) {
                mFences->remove(fencename);
                mFenceOwners->remove(fencename);
            }
        }
    }

    return 0;
}

int _FenceCenter::processReleaseReadOwner(String fencename,DistributeLinker linker) {
    AutoLock l(mMutex);
    auto info = mFences->get(fencename);
    if(info == nullptr) {
        return -EINVAL;
    }

    auto fence = Cast<ReadWriteFence>(info->fence);
    if(fence == nullptr) {
        return -EINVAL;
    }

    auto readfence = fence->getReadFence();
    auto writefence = fence->getWriteFence();
    //check whether this linker is owner of readlock
    if(!readfence->removeOwner(linker)) {
        return -ECANCELED;
    }

    if(readfence->getCount() == 0) {
        auto waiters = info->waiters;
        writefence->setOwner(nullptr);

        while(waiters->size() != 0) {
            auto waiter = waiters->takeFirst();
            auto nextLinker = mLinkers->get(waiter->waiterToken);
            if(nextLinker == nullptr || sendResponse(fencename,nextLinker) < 0) {
                continue;
            }

            writefence->setOwner(waiter->waiterToken);
            writefence->incCount();
            break;
        }

        if(writefence->getOwner() == nullptr) {
            //no waiter and owner,remove this lock
            mFences->remove(fencename);
        }
    }
    return 0;
}

int _FenceCenter::processReleaseWriteOwner(String fencename,DistributeLinker linker)  {
    AutoLock l(mMutex);
    auto info = mFences->get(fencename);

    if(info == nullptr) {
        return -EINVAL;
    }
    auto fence = Cast<ReadWriteFence>(info->fence);

    if(fence == nullptr) {
        return -EINVAL;
    }

    auto writefence = fence->getWriteFence();
    if(writefence->getOwner() != genToken(linker)) {
        return -ECANCELED;
    }
    auto waiters = info->waiters;
    writefence->decCount();
    
    if(writefence->isFree()) {
        writefence->setOwner(nullptr);
        while(waiters->size() != 0) {
            auto waiter = waiters->takeFirst();
            bool isFoundOwner = false;

            auto waiterLinker = mLinkers->get(waiter->waiterToken);
            if(waiter == nullptr || sendResponse(fencename,waiterLinker) < 0) {
                continue;
            } 

            if(waiter->flag == st(FenceWaiter)::ReadFlag) {
                auto iterator = waiters->getIterator();
                while(iterator->hasValue()) {
                    auto fencewaiter = iterator->getValue();
                    if(fencewaiter->flag == st(FenceWaiter)::ReadFlag) {
                        auto nextwaiter = iterator->getValue();
                        auto nextLinker = mLinkers->get(nextwaiter->waiterToken);
                        if(nextLinker != nullptr && sendResponse(fencename,nextLinker) > 0) {
                            isFoundOwner = true;
                        }
                        iterator->remove();
                    }
                }
            } else {
                auto nextLinker = mLinkers->get(waiter->waiterToken);
                if(sendResponse(fencename,nextLinker) > 0) {
                    isFoundOwner = true;
                }
            }

            if(!isFoundOwner) {
                continue;
            }
        }
    }
    return 0;
}

int _FenceCenter::sendResponse(String fencename,DistributeLinker linker,int result) {
    auto response = ConfirmFenceMessage::New(fencename,result);
    return linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
}

int _FenceCenter::onNewClient(DistributeLinker linker) {
    //TODO
    return -1;
}

int _FenceCenter::onDisconnectClient(DistributeLinker linker) {
    String token = genToken(linker);
    AutoLock l(mMutex);
    mLinkers->remove(genToken(linker));
    ForEveryOne(ownerpair,mFenceOwners) {
        //we should check wether this connection acquired fence
        if(ownerpair->getValue()->equals(token)) {
            //clear count!
            auto info = mFences->get(ownerpair->getKey());
            info->fence->setCount(1);
            processReleaseOwner(ownerpair->getKey(),token);
            break;
        }
    }

    ForEveryOne(fencepair,mFences) {
        auto fenceinfo = fencepair->getValue();
        auto fencename = fencepair->getKey();

        if(IsInstance(ReadWriteFence,fenceinfo->fence)) {
            auto rwfence = Cast<ReadWriteFence>(fenceinfo->fence);
            processReleaseWriteOwner(fencename,linker);
            processReleaseReadOwner(fencename,linker);
        }
    }
    return 0;
}

FenceInfo _FenceCenter::getIfEmptyCreate(FenceMessage msg) {
    AutoLock l(mMutex);
    auto info = mFences->get(msg->getFenceName());
    if(info == nullptr) {
        info = FenceInfo::New(msg->getFenceName());
        switch(msg->getEvent()) {
            case st(FenceMessage)::ApplyReadFence:
            case st(FenceMessage)::ApplyWriteFence:
            info->fence = ReadWriteFence::New(msg->getFenceName());
            break;
        }
        mFences->put(msg->getFenceName(),info);
    }
    return info;
}

}

