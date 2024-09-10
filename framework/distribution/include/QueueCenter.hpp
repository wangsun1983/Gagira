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
#include "UnitTestInterface.hpp"

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

    DECLARE_UNIT_TEST_INTERFACE(
        uint64_t getRemainTaskSize();
        uint64_t getRemainWaiterSize();
        void setDelayProcessInterval(uint32_t);
    )

private:
    QueueMessage getNextMessage(DistributeLinker linker,uint32_t req_id);
    QueueMessage getNextClient();
    void addProcessingMessageLocked(DistributeLinker,QueueMessage);
    void removeProcessingMessageLocked(DistributeLinker,uint32_t req_id);
    QueueMessage getProcessingMessageLocked(DistributeLinker,uint32_t req_id);
    bool handlePermission(DistributeLinker,QueueMessage);

    DistributeHandler mHandler;

    DistributeMessageConverter mConverter;
    Mutex mMutex;
    LinkedList<QueueWaitReceiver> mWaiters;
    LinkedList<QueueMessage> mTasks;
    HashMap<DistributeLinker,HashMap<Uint32,QueueMessage>> mProcessingMessages;
    volatile int32_t isRunning;

    //For Testing
    uint32_t TestDelayProcessInterval = 0;
};

}

#endif
