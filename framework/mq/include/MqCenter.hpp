#ifndef __GAGRIA_MQ_CENTOR_HPP__
#define __GAGRIA_MQ_CENTOR_HPP__

#include "String.hpp"
#include "SocketMonitor.hpp"
#include "SocketListener.hpp"
#include "ReadWriteLock.hpp"
#include "InetAddress.hpp"
#include "ByteRingArray.hpp"
#include "ByteRingArrayReader.hpp"
#include "Condition.hpp"
#include "MqMessage.hpp"
#include "MqPersistenceInterface.hpp"
#include "ThreadScheduledPoolExecutor.hpp"
#include "Random.hpp"
#include "ConcurrentHashMap.hpp"
#include "ConcurrentQueue.hpp"
#include "CountDownLatch.hpp"

using namespace obotcha;

namespace gagira {

class _MqCenter;

DECLARE_CLASS(MqChannelGroup) {
public:
    _MqChannelGroup(String);
    ConcurrentQueue<OutputStream> mStreams;
    String mChannel;
};

DECLARE_CLASS(MqClientInfo) {
public:
    _MqClientInfo(int buffsize);
    ByteRingArray mBuffer;
    ByteRingArrayReader mReader;
    uint32_t mCurrentMsgLen;
};

DECLARE_CLASS(MqCenter) IMPLEMENTS(SocketListener) {
public:
    friend class _MqWorker;
    _MqCenter(String url,
              int workers,
              int buffsize,
              int acktimeout,
              int retryTimes,
              int retryInterval);

    void waitForExit(long interval = 0);
    int start();
    int close();
    ~_MqCenter();

private:
    SocketMonitor mSocketMonitor;

    void onSocketMessage(int,Socket,ByteArray);
    int dispatchMessage(Socket sock,ByteArray);

    int processSubscribe(MqMessage);
    int processUnSubscribe(MqMessage);

    int processOneshot(MqMessage);
    int processPublish(MqMessage);
    int processAck(MqMessage);
    int setAcknowledgeTimer(MqMessage msg);

    int processStick(MqMessage msg);

    InetAddress mAddrss;
    ServerSocket mServerSock;

    ConcurrentHashMap<String,MqChannelGroup> mChannelGroups;
    ConcurrentHashMap<Socket,MqClientInfo> mClients;

    //<Channel,HashMap<Tag,MqMessage>>;
    Mutex mStickyMutex;
    HashMap<String,HashMap<String,MqMessage>> mStickyMessages;

    CountDownLatch mExitLatch;

    HashMap<String,Future> mAckTimerFuture;

    MqPersistenceInterface mPersistence;

    //uint32_t mCurrentMsgLen;
    int mAckTimeout;
    int mRetryTimes;
    int mRetryInterval;
    int mBuffSize;

    ThreadScheduledPoolExecutor mTimer;
};

}

#endif
