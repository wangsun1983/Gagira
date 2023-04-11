#ifndef __GAGRIA_QUEUE_CENTOR_HPP__
#define __GAGRIA_QUEUE_CENTOR_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "SocketMonitor.hpp"
#include "SpaceOption.hpp"
#include "InetAddress.hpp"
#include "ServerSocket.hpp"
#include "DistributeCenter.hpp"
#include "DistributeOption.hpp"
#include "DistributeMessageConverter.hpp"
#include "BlockingLinkedList.hpp"
#include "QueueMessage.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(QueueCenter) IMPLEMENTS(DistributeCenter,Thread) {
public:
    _QueueCenter(String url,DistributeOption);
    ~_QueueCenter();

    int onMessage(DistributeLinker,ByteArray);
    int onNewClient(DistributeLinker);
    int onDisconnectClient(DistributeLinker);
    
    int start();
    int close();
private:
    DistributeMessageConverter mConverter;

    BlockingLinkedList<DistributeLinker> mClients;
    BlockingLinkedList<QueueMessage> mTasks;
    volatile int32_t isRunning;

    void run();
};

}

#endif
