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

using namespace obotcha;

namespace gagira {

class _MqCenter;

DECLARE_CLASS(MqWorker) IMPLEMENTS(Thread) {
public:
    _MqWorker(_MqCenter *c);

    ~_MqWorker();
    
    void close();

    void enqueueMessage(MqMessage msg);

    void run();

    int size();

private:
    _MqCenter *center;
    BlockingLinkedList<MqMessage> actions;
};

DECLARE_CLASS(MqStreamGroup) {
public:
    _MqStreamGroup();
    ReadWriteLock mReadWriteLock;
    ReadLock mRdLock;
    WriteLock mWrLock;
    ArrayList<OutputStream> streams;
    Random mRand;

    int doSubscribe(MqMessage);
    int doOneshot(MqMessage);
    int doPublish(MqMessage);
    int doAck(MqMessage);
};

DECLARE_CLASS(MqCenter) IMPLEMENTS(SocketListener) {
public:
    friend class _MqWorker;
    _MqCenter(String url,
              int workers,
              int buffsize,
              MqPersistenceInterface component,
              int acktimeout,
              int retryTimes,
              int retryInterval);

    void waitForExit(long interval = 0);
    int close();
    ~_MqCenter();

private:
    SocketMonitor monitor;

    void onSocketMessage(int,Socket,ByteArray);
    void dispatchMessage(Socket sock,ByteArray);

    int processSubscribe(MqMessage);
    int processOneshot(MqMessage);
    int processPublish(MqMessage);
    int processAck(MqMessage);
    int setAcknowledgeTimer(MqMessage msg);

    InetAddress mAddrss;
    ServerSocket sock;

    ReadWriteLock mStreamRwLock;
    ReadLock mStreamReadLock;
    WriteLock mStreamWriteLock;
    HashMap<String,MqStreamGroup> mStreams;

    ByteRingArray mBuffer;
    ByteRingArrayReader mReader;

    Mutex mMutex;
    Condition waitExit;

    //worker
    ReadWriteLock mWorkerReadWriteLock;
    ReadLock mWorkerReadLock;
    WriteLock mWorkerWriteLock;
    ArrayList<MqWorker> mWorkers;
    HashMap<String,MqWorker> mMqWorkerMap;
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
