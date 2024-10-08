#ifndef __GAGRIA_DISTRIBUTE_CENTOR_HPP__
#define __GAGRIA_DISTRIBUTE_CENTOR_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "SocketMonitor.hpp"
#include "SocketListener.hpp"
#include "InetAddress.hpp"
#include "ThreadScheduledPoolExecutor.hpp"
#include "Random.hpp"
#include "ConcurrentHashMap.hpp"
#include "ConcurrentQueue.hpp"
#include "CountDownLatch.hpp"
#include "DistributeLinker.hpp"
#include "DistributeOption.hpp"
#include "UUID.hpp"
#include "Sha.hpp"
#include "Base64.hpp"
#include "ThreadScheduledPoolExecutor.hpp"
#include "ServerSocket.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DistributeCenter) IMPLEMENTS(SocketListener) {
public:
    friend class _MqWorker;
    _DistributeCenter(String url,DistributeOption);
    DistributeOption getOption();
    void waitForExit(long interval = 0);
    ~_DistributeCenter();

    bool isClosed();
    
    virtual int start();
    virtual int close();
    virtual int onMessage(DistributeLinker,ByteArray) = 0;
    virtual int onNewClient(DistributeLinker) = 0;
    virtual int onDisconnectClient(DistributeLinker) = 0;

protected:
    InetAddress mAddress;
    DistributeLinker getLinker(Socket);

private:
    SocketMonitor mSocketMonitor;
    void onSocketMessage(st(Net)::Event,Socket,ByteArray);
    ServerSocket mServerSock;
    ConcurrentHashMap<Socket,DistributeLinker> mClients;
    CountDownLatch mExitLatch;
    DistributeOption mOption;
};

}

#endif
