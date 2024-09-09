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
    mSocketMonitor = SocketMonitor::New();
    mAddress = HttpUrl::New(url)->getInetAddress()->get(0);
    mClients = ConcurrentHashMap<Socket,DistributeLinker>::New();
    mExitLatch = CountDownLatch::New(1);
    mOption = (option == nullptr)?DistributeOption::New():option;
    mServerSock = SocketBuilder::New()->setAddress(mAddress)->newServerSocket();
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

void _DistributeCenter::onSocketMessage(st(Net)::Event event,Socket sock,ByteArray data) {
    switch(event) {
        case st(Net)::Event::Connect: {
            auto client = DistributeLinker::New(sock,mOption->getClientRecvBuffSize());
            mClients->put(sock,client);
            onNewClient(client);
        }
        break;

        case st(Net)::Event::Message: {
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

        case st(Net)::Event::Disconnect: {
            auto client = mClients->get(sock);
            onDisconnectClient(client);
            mClients->remove(sock);
        }
        break;
    }
}

DistributeLinker _DistributeCenter::getLinker(Socket sock) {
    return mClients->get(sock);
}

int _DistributeCenter::close() {
    Inspect(isClosed(),0)

    if(mServerSock != nullptr) {
        mSocketMonitor->unbind(mServerSock,true);
        mServerSock->close();
        mServerSock = nullptr;
    }

    if(mSocketMonitor != nullptr) {
        mSocketMonitor->close();
        mSocketMonitor->waitForExit();
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

bool _DistributeCenter::isClosed() {
    return mExitLatch->getCount() == 0;
}

}
