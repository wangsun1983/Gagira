#ifndef __GAGRIA_MQ_CENTOR_HPP__
#define __GAGRIA_MQ_CENTOR_HPP__

#include <atomic>

#include "Object.hpp"
#include "String.hpp"
#include "SocketMonitor.hpp"
#include "SocketListener.hpp"
#include "InetAddress.hpp"
#include "MqMessage.hpp"
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
#include "MqDLQMessage.hpp"
#include "Base64.hpp"
#include "ThreadScheduledPoolExecutor.hpp"
#include "DistributeMessageConverter.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqCenter) IMPLEMENTS(DistributeCenter) {
public:
    friend class _MqWorker;
    _MqCenter(String url,DistributeOption);
    int start();
    int close();
    ~_MqCenter();

private:
    SocketMonitor mSocketMonitor;
    DistributeMessageConverter mConverter;

    int onMessage(DistributeLinker,ByteArray);
    int onNewClient(DistributeLinker);
    int onDisconnectClient(DistributeLinker);

    int dispatchMessage(Socket sock,ByteArray);
    int processSubscribe(MqMessage);
    int processUnSubscribe(MqMessage);
    int processPublish(MqMessage);
    int processAck(MqMessage);
    int processPostBack(MqMessage);
    int processSubscribePersistence(MqMessage);
    int processSubscribeDLQ(MqMessage);
    
    int registWaitAckTask(MqMessage msg);

    bool processSendFailMessage(MqDLQMessage msg);
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
