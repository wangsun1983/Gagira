#ifndef __GAGRIA_BROADCAST_CENTOR_HPP__
#define __GAGRIA_BROADCAST_CENTOR_HPP__

#include <atomic>

#include "Object.hpp"
#include "String.hpp"
#include "SocketMonitor.hpp"
#include "SocketListener.hpp"
#include "InetAddress.hpp"
#include "BroadcastMessage.hpp"
#include "ThreadScheduledPoolExecutor.hpp"
#include "Random.hpp"
#include "ConcurrentHashMap.hpp"
#include "ConcurrentQueue.hpp"
#include "CountDownLatch.hpp"
#include "DistributeOption.hpp"
#include "DistributeLinker.hpp"
#include "DistributeCenter.hpp"
#include "UUID.hpp"
#include "Sha.hpp"
#include "BroadcastDLQMessage.hpp"
#include "Base64.hpp"
#include "ThreadScheduledPoolExecutor.hpp"
#include "DistributeMessageConverter.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(BroadcastCenter) IMPLEMENTS(DistributeCenter) {
public:
    friend class _BroadcastWorker;
    _BroadcastCenter(String url,DistributeOption);
    int start();
    int close();
    ~_BroadcastCenter();

private:
    SocketMonitor mSocketMonitor;
    DistributeMessageConverter mConverter;

    int onMessage(DistributeLinker,ByteArray);
    int onNewClient(DistributeLinker);
    int onDisconnectClient(DistributeLinker);

    int dispatchMessage(Socket sock,ByteArray);
    int processSubscribe(BroadcastMessage);
    int processUnSubscribe(BroadcastMessage);
    int processPublish(BroadcastMessage);
    int processAck(BroadcastMessage);
    int processPostBack(BroadcastMessage);
    int processSubscribePersistence(BroadcastMessage);
    int processSubscribeDLQ(BroadcastMessage);
    
    int registWaitAckTask(BroadcastMessage msg);

    bool processSendFailMessage(BroadcastDLQMessage msg);
    ConcurrentHashMap<String,ArrayList<OutputStream>> mChannelGroups;
    ConcurrentHashMap<String,HashMap<String,ByteArray>> mStickyMessages;
    ThreadScheduledPoolExecutor mWaitAckThreadPools;
    ConcurrentHashMap<String,Future> mWaitAckMessages;
    ThreadScheduledPoolExecutor mSchedulePool;

    ReadWriteLock mPersistRwLock;
    ReadLock mPersistRLock;
    WriteLock mPersistWLock;
    Socket mPersistenceClient;
    std::atomic<bool> mPostBackCompleted;

    Mutex mDlqMutex;
    Socket mDlqClient;
};

}

#endif
