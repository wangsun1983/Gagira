#include "DistributeCenter.hpp"
#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "NetEvent.hpp"
#include "ForEveryOne.hpp"
#include "System.hpp"
#include "ExecutorBuilder.hpp"
#include "Log.hpp"
#include "Inspect.hpp"
#include "Synchronized.hpp"
#include "Md.hpp"

using namespace obotcha;

namespace gagira {

_DistributeCenter::_DistributeCenter(String url,DistributeOption option) {
    mSocketMonitor = createSocketMonitor();
    mAddress = createHttpUrl(url)->getInetAddress()->get(0);
    mClients = createConcurrentHashMap<Socket,DistributeLinker>();
    mExitLatch = createCountDownLatch(1);
    mOption = (option == nullptr)?createDistributeOption():option;
    mServerSock = createSocketBuilder()->setAddress(mAddress)->newServerSocket();
}

DistributeOption _DistributeCenter::getOption() {
    return mOption;
}

int _DistributeCenter::start() {
    Inspect(mServerSock->bind() < 0 
            || mSocketMonitor->bind(mServerSock,AutoClone(this)) < 0,
            -1);
    return 0;
}

void _DistributeCenter::onSocketMessage(int event,Socket sock,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Connect: {
            auto client = createDistributeLinker(sock,mOption->getClientRecvBuffSize());
            mClients->put(sock,client);
            onNewClient(client);
        }
        break;

        case st(NetEvent)::Message: {
            auto client = mClients->get(sock);
            if(client == nullptr) {
                LOG(ERROR)<<"Received a message,but can not find it's connection";
                return;
            }

            ArrayList<ByteArray> result = client->doParse(data);
            if(result != nullptr && result->size() != 0) {
                ForEveryOne(msg,result) {
                    onMessage(client,msg);
                }
            }
        }
        break;

        case st(NetEvent)::Disconnect: {
            auto client = mClients->get(sock);
            onDisconnectClient(client);
            mClients->remove(sock);
        }
        break;
    }
}

int _DistributeCenter::close() {
    if(mServerSock != nullptr) {
        mServerSock->close();
        mSocketMonitor->unbind(mServerSock);
        mServerSock = nullptr;
    }

    if(mSocketMonitor != nullptr) {
        mSocketMonitor->close();
        mSocketMonitor = nullptr;
    }

    mClients->clear();
    mExitLatch->countDown();
    return 0;
}

void _DistributeCenter::waitForExit(long interval) {
    mExitLatch->await(interval);
}

_DistributeCenter::~_DistributeCenter() {
    this->close();
}

}
