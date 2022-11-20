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
#include "MqOption.hpp"
#include "MqLinker.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqCenter) IMPLEMENTS(SocketListener) {
public:
    friend class _MqWorker;
    _MqCenter(String url,MqOption);

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
    int processStick(MqMessage msg);

    InetAddress mAddress;
    ServerSocket mServerSock;

    ConcurrentHashMap<String,ArrayList<OutputStream>> mChannelGroups;

    ConcurrentHashMap<Socket,MqLinker> mClients;

    ConcurrentHashMap<String,HashMap<String,ByteArray>> mStickyMessages;

    CountDownLatch mExitLatch;

    MqPersistenceInterface mPersistence;

    MqOption mOption;
};

}

#endif
