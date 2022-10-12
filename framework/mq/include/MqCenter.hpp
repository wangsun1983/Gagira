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

using namespace obotcha;

namespace gagira {

class _MqCenter;

DECLARE_CLASS(MqStreamGroup) {
public:
    _MqStreamGroup(String);
    ConcurrentQueue<OutputStream> mStreams;
    String mChannel;
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
    SocketMonitor monitor;

    void onSocketMessage(int,Socket,ByteArray);
    void dispatchMessage(Socket sock,ByteArray);

    int processSubscribe(MqMessage);
    int processUnSubscribe(MqMessage);
    
    int processOneshot(MqMessage);
    int processPublish(MqMessage);
    int processAck(MqMessage);
    int setAcknowledgeTimer(MqMessage msg);

    InetAddress mAddrss;
    ServerSocket sock;

    ConcurrentHashMap<String,MqStreamGroup> mStreams;

    ByteRingArray mBuffer;
    ByteRingArrayReader mReader;

    Mutex mMutex;
    Condition waitExit;

    HashMap<String,Future> mAckTimerFuture;

    MqPersistenceInterface mPersistence;

    int mCurrentMsgLen;
    int mAckTimeout;
    int mRetryTimes;
    int mRetryInterval;

    ThreadScheduledPoolExecutor mTimer;
};

}

#endif
