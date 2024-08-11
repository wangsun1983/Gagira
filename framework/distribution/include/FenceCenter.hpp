#ifndef __GAGRIA_FENCE_CENTOR_HPP__
#define __GAGRIA_FENCE_CENTOR_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "DistributeCenter.hpp"
#include "DistributeOption.hpp"
#include "DistributeLinker.hpp"
#include "DistributeHandler.hpp"
#include "DistributeMessageConverter.hpp"
#include "Fence.hpp"
#include "LinkedList.hpp"
#include "ThreadScheduledPoolExecutor.hpp"
#include "FenceMessage.hpp"
#include "FenceOption.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(FenceWaiter) {
public:
    enum {
        ReadFlag = 0,
        WriteFlag,
        MutexFlag,
    };

    _FenceWaiter(String toke,int flag = MutexFlag);
    String waiterToken;
    int flag;
};

DECLARE_CLASS(FenceInfo) {
public:
    _FenceInfo(String);
    Mutex mutex;
    Fence fence;
    LinkedList<FenceWaiter> waiters;
};

DECLARE_CLASS(FenceCenter) IMPLEMENTS(DistributeCenter) {
public:
    _FenceCenter(String url,FenceOption option = nullptr);
    ~_FenceCenter();
    int onMessage(DistributeLinker,ByteArray);
    int onNewClient(DistributeLinker);
    int onDisconnectClient(DistributeLinker);

private:
    String genToken(DistributeLinker);
    int sendResponse(String fencename,DistributeLinker,int result = 0);

    int processReleaseOwner(String fencename,String linkertoken);
    int processReleaseReadOwner(String fencename,DistributeLinker linker);
    int processReleaseWriteOwner(String fencename,DistributeLinker linker);

    FenceInfo getIfEmptyCreate(FenceMessage);

    DistributeMessageConverter mConverter;
    Mutex mMutex;
    HashMap<String,FenceInfo> mFences; //<fence name,FenceInfo>
    HashMap<String,DistributeLinker> mLinkers;
    HashMap<String,String> mFenceOwners; //<fence name,token>

    ThreadScheduledPoolExecutor mScheduledPool;

    DistributeHandler mHandler;
};

}

#endif
