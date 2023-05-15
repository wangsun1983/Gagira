#include "FenceCenter.hpp"
#include "FenceMessage.hpp"
#include "AutoLock.hpp"
#include "ExecutorBuilder.hpp"
#include "ForEveryOne.hpp"
#include "Log.hpp"

using namespace obotcha;

namespace gagira {

//FenceWaiter
_FenceWaiter::_FenceWaiter(String token,int flag) {
    this->waiterToken = token;
    this->flag = flag;
}

//FenceInfo
_FenceInfo::_FenceInfo(String name) {
    mutex = createMutex();
    fence = createFence(name);
    waiters = createLinkedList<FenceWaiter>();
}

//FenceCenter
_FenceCenter::_FenceCenter(String url,DistributeOption option):_DistributeCenter(url,option) {
    mFences = createHashMap<String,FenceInfo>();
    mLinkers = createHashMap<String,DistributeLinker>();
    mFenceOwners = createHashMap<String,String>();
    mMutex = createMutex();
    mScheduledPool = createExecutorBuilder()->setMaxThreadNum(2)->newScheduledThreadPool();
}

_FenceCenter::~_FenceCenter() {
    //TODO
}

String _FenceCenter::genToken(DistributeLinker linker) {
    auto address = linker->getSocket()->getInetAddress();
    return address->getAddress()->append(createString(address->getPort()));
}

int _FenceCenter::onMessage(DistributeLinker linker,ByteArray data) {
    auto msg = mConverter->generateMessage<FenceMessage>(data);
    String linkerToken = genToken(linker);

    switch(msg->event) {
        case st(FenceMessage)::ApplyReadFence: {
            AutoLock l(mMutex);
            mLinkers->put(linkerToken,linker);
            auto info = mFences->get(msg->fencename);
            if(info == nullptr) {
                info = createFenceInfo(msg->fencename);
                info->fence = createReadWriteFence(msg->fencename);
                mFences->put(msg->fencename,info);
            }

            //check writeloc
            auto fence = Cast<ReadWriteFence>(info->fence);
            auto readfence = fence->getReadFence();
            if(readfence->getCount() != 0) {
                //readfence->addCount(1);
                readfence->addOwner(linker);
                auto response = createConfirmFenceMessage(msg->fencename);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
            } else {
                //check write fence
                auto writeFence = fence->getWriteFence();
                if(writeFence->getOwner() != nullptr) {
                    info->waiters->putLast(createFenceWaiter(linkerToken,st(FenceWaiter)::ReadFlag));
                } else {
                    //readfence->addCount(1);
                    readfence->addOwner(linker);
                    auto response = createConfirmFenceMessage(msg->fencename);
                    linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                }
            }
        } break;

        case st(FenceMessage)::ApplyReleaseReadFence: {
            AutoLock l(mMutex);
            auto info = mFences->get(msg->fencename);
            if(info == nullptr) {
                //TODO
                break;
            }

            auto fence = Cast<ReadWriteFence>(info->fence);
            auto readfence = fence->getReadFence();
            readfence->removeOwner(linker);
            if(readfence->getCount() == 0) {
                auto waiters = info->waiters;
                if(waiters->size() != 0) {
                    while(waiters->size() != 0) {
                        auto waiter = waiters->takeFirst();
                        auto nextLinker = mLinkers->get(waiter->waiterToken);
                        if(nextLinker == nullptr) {
                            continue;
                        }
                        auto writefence = fence->getWriteFence();
                        writefence->setOwner(nextLinker);
                        
                        auto response = createConfirmFenceMessage(msg->fencename);
                        int ret = nextLinker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                        if(ret < 0) {
                            //TODO
                            continue;
                        }
                        break;
                    }
                } else {
                    //no waiter and owner,remove this lock
                    mFences->remove(msg->fencename);
                }
            }
            auto response = createConfirmFenceMessage(msg->fencename);
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(FenceMessage)::ApplyWriteFence: {
            AutoLock l(mMutex);
            printf("apply write fence trace1\n");
            mLinkers->put(linkerToken,linker);
            auto info = mFences->get(msg->fencename);
            if(info == nullptr) {
                info = createFenceInfo(msg->fencename);
                info->fence = createReadWriteFence(msg->fencename);
                mFences->put(msg->fencename,info);
            }
            printf("apply write fence trace2\n");
            auto fence = Cast<ReadWriteFence>(info->fence);
            auto readfence = fence->getReadFence();
            printf("apply write fence trace3,count is %d\n",readfence->getCount());
            if(readfence->getCount() != 0) {
                info->waiters->putLast(createFenceWaiter(linkerToken,st(FenceWaiter)::WriteFlag));
            } else {
                auto writeFence = fence->getWriteFence();
                auto owner = writeFence->getOwner();
                if(owner != nullptr && !owner->equals(linkerToken)) {
                    info->waiters->putLast(createFenceWaiter(linkerToken,st(FenceWaiter)::WriteFlag));
                } else {
                    writeFence->setOwner(linkerToken);
                    auto response = createConfirmFenceMessage(msg->fencename);
                    linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                }
            }
            printf("apply write fence trace4 \n");
        } break;

        case st(FenceMessage)::ApplyReleaseWriteFence: {
            AutoLock l(mMutex);
            auto info = mFences->get(msg->fencename);
            if(info == nullptr) {
                //TODO
                break;
            }

            auto fence = Cast<ReadWriteFence>(info->fence);
            auto waiters = info->waiters;
            if(waiters->size() != 0) {
                auto waiter = waiters->takeFirst();
                auto response = createConfirmFenceMessage(msg->fencename);
                auto newlinker = mLinkers->get(waiter->waiterToken);
                int ret = newlinker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                //TODO
                if(waiter->flag == st(FenceWaiter)::ReadFlag) {
                    auto iterator = waiters->getIterator();
                    while(iterator->hasValue()) {
                        auto fencewaiter = iterator->getValue();
                        if(fencewaiter->flag == st(FenceWaiter)::ReadFlag) {
                            auto nextwaiter = iterator->getValue();
                            auto nextLinker = mLinkers->get(nextwaiter->waiterToken);
                            auto response = createConfirmFenceMessage(msg->fencename);
                            nextLinker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                            iterator->remove();
                        }
                    }
                }
            }
            auto response = createConfirmFenceMessage(msg->fencename);
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(FenceMessage)::ApplyFence: {
            AutoLock l(mMutex);
            mLinkers->put(linkerToken,linker);
            
            auto info = mFences->get(msg->fencename);
            if(info == nullptr) {
                info = createFenceInfo(msg->fencename);
                mFences->put(msg->fencename,info);
            }

            auto fence = info->fence;
            auto owner = fence->getOwner();
            if(owner != nullptr && !owner->equals(linkerToken)) {
                //add to wait list
                info->waiters->putLast(createFenceWaiter(linkerToken));
                auto waittime = msg->getWaitTime();
                printf("FenceCenter,waittime is %ld \n",waittime);
                if(waittime != 0) {
                    mScheduledPool->schedule(waittime,[&](String token,String name) {
                        printf("FenceCenter,mScheduledPool trace1 \n");
                        DistributeLinker linker = nullptr;
                        ConfirmFenceMessage response = nullptr;
                        {
                            AutoLock l(mMutex);
                            linker = mLinkers->remove(token);
                            response = createConfirmFenceMessage(name,ETIMEDOUT);
                        }
                        printf("FenceCenter,mScheduledPool trace2 \n");
                        linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                    },linkerToken,fence->getName());
                }
            } else {
                //acquire lock success
                fence->setOwner(linkerToken);
                mFenceOwners->put(msg->fencename,linkerToken);
                auto response = createConfirmFenceMessage(msg->fencename);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
            }
        } break;

        case st(FenceMessage)::ApplyReleaseFence: {
            int ret = processReleaseOwner(msg->fencename,linkerToken);
            auto response = createConfirmFenceMessage(msg->fencename,ret);
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;
    }
    return -1;
}

int _FenceCenter::processReleaseOwner(String fencename,String linkertoken) {
    printf("processReleaseOwner trace1 \n");
    AutoLock l(mMutex);
    auto owner = mFenceOwners->get(fencename);
    if(owner == nullptr || !owner->equals(linkertoken)) {
        return EINVAL;
    }
    printf("processReleaseOwner trace2 \n");
    auto info = mFences->get(fencename);
    if(info != nullptr) {
        AutoLock l(info->mutex);
        while(info->waiters->size() != 0) {
            auto nextWaiterToken = info->waiters->takeFirst()->waiterToken;
            auto nextWaiter = mLinkers->get(nextWaiterToken);
            if(nextWaiter == nullptr) {
                continue;
            }

            auto response = createConfirmFenceMessage(fencename);
            int ret = nextWaiter->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
            if(ret < 0) {
                continue;
            }

            mFenceOwners->put(fencename,genToken(nextWaiter));
        }

        if(info->waiters->size() == 0) {
            mFences->remove(fencename);
            mFenceOwners->remove(fencename);
        }
    }

    return 0;
}

int _FenceCenter::processReleaseReadOwner(String fencename,String linkertoken) {
    //AutoLock l(mMutex);
    //auto info = mFences->get(fencename);

    return 0;
}

int _FenceCenter::processReleaseWriteOwner(String fencename,String linkertoken)  {
    //TODO
    return 0;
}


int _FenceCenter::onNewClient(DistributeLinker linker) {
    //TODO
    return -1;
}

int _FenceCenter::onDisconnectClient(DistributeLinker linker) {
    String token = genToken(linker);
    AutoLock l(mMutex);
    ForEveryOne(pair,mFenceOwners) {
        //we should check wether this connection acquired fence
        if(pair->getValue()->equals(token)) {
            processReleaseOwner(pair->getKey(),token);
            break;
        }
    }
    return 0;
}

}

