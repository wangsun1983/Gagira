#ifndef __GAGRIA_QUEUE_CENTOR_HPP__
#define __GAGRIA_QUEUE_CENTOR_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "SocketMonitor.hpp"
#include "MapOption.hpp"
#include "InetAddress.hpp"
#include "ServerSocket.hpp"
#include "DistributeCenter.hpp"
#include "DistributeOption.hpp"
#include "DistributeMessageConverter.hpp"
#include "BlockingLinkedList.hpp"
#include "QueueMessage.hpp"
#include "DistributeHandler.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(QueueWaitReceiver) {
public:
    _QueueWaitReceiver(uint32_t,DistributeLinker);
    uint32_t req_id;
    DistributeLinker linker;
};

DECLARE_CLASS(QueueCenter) IMPLEMENTS(DistributeCenter) {
public:
    _QueueCenter(String url,DistributeOption);
    ~_QueueCenter();

    int onMessage(DistributeLinker,ByteArray);
    int onNewClient(DistributeLinker);
    int onDisconnectClient(DistributeLinker);
    
    int start();
    int close();
private:
    QueueMessage getNextMessage(DistributeLinker linker,uint32_t req_id);
    QueueMessage getNextClient();
    void addResponseWaitersLocked(DistributeLinker,QueueMessage);
    void removeResponseWaiterLocked(DistributeLinker,uint32_t req_id);
    QueueMessage getResponseWaitersLocked(DistributeLinker,uint32_t req_id);

    DistributeHandler mHandler;

    DistributeMessageConverter mConverter;
    Mutex mMutex;
    LinkedList<QueueWaitReceiver> mWaiters;
    LinkedList<QueueMessage> mTasks;
    HashMap<DistributeLinker,HashMap<Uint32,QueueMessage>> mResponseWaiters;
    volatile int32_t isRunning;

    //void run();
};

}

#endif
