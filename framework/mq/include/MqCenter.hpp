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
#include "MqPersistentComponent.hpp"
#include "MqUndeliveredComponent.hpp"

using namespace obotcha;

namespace gagira {

class _MqCenter;

DECLARE_CLASS(MqPersistInterface) {
public:
    void onNewChannel(String channel);
    void onNewMessage(String channel,Message msg);
    Message takeOneMessage(String channel);
};

DECLARE_CLASS(MqWorker) IMPLEMENTS(Thread) {
public:
    _MqWorker(_MqCenter *c);

    void enqueueMessage(MqMessage msg);

    void run();

    int size();

private:
    _MqCenter *center;
    BlockingLinkedList<MqMessage> actions;
    Mutex mMutex;
    HashMap<String,Integer> mChannelCounts;
    HashMap<String,Integer> mQueueProcessorIndexs;
    bool isRunning;
};

DECLARE_CLASS(MqCenter) IMPLEMENTS(SocketListener) {
public:

    friend class _MqWorker;
    _MqCenter(String url,int workers,int buffsize,MqPersistentComponent component,int acktimeout);

    void waitForExit(long interval = 0);

    int close();

private:
    static SocketMonitor monitor;

    void onSocketMessage(int,Socket,ByteArray);
    void dispatchMessage(Socket sock,ByteArray);

    ArrayList<OutputStream> getOutputStreams(String);
    void setOutputStreams(String channel,ArrayList<OutputStream> list);
    void addStickyMessage(String,ByteArray);
    ByteArray getStickyMessage(String);

    void removeWorkerRecord(String channel);

    InetAddress mAddrss;
    ServerSocket sock;

    ReadWriteLock mReadWriteLock;
    ReadLock mReadLock;
    WriteLock mWriteLock;
    HashMap<String,ArrayList<OutputStream>> mOutputStreams;

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

    MqPersistentComponent mPersistentComp;

    MqUndeliveredComponent mUndeliveredComp; 

    int mCurrentMsgLen;

    int mAckTimeout;
};

}

#endif
