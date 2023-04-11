#include <mutex>

#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "SpaceConnection.hpp"
#include "SpaceMessage.hpp"
#include "NetEvent.hpp"
#include "InitializeException.hpp"
#include "ForEveryOne.hpp"
#include "Inspect.hpp"

using namespace obotcha;

namespace gagira {

//---------- SpaceConnection
_SpaceConnection::_SpaceConnection(String s,SpaceConnectionListener l) {
    HttpUrl url = createHttpUrl(s);
    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find MqCenter");
    }

    mListener = l;
    mSocketMonitor = createSocketMonitor(1);
    mParser = createDistributeMessageParser(1024*4);
    mConverter = createDistributeMessageConverter();
}

int _SpaceConnection::connect() {
    mSock = createSocketBuilder()->setAddress(mAddress)->newSocket();
    Inspect(mSock->connect() < 0,-1);

    mInput = mSock->getInputStream();
    mOutput = mSock->getOutputStream();

    if(mListener != nullptr) {
        mListener->onConnect();
    }

    return mSocketMonitor->bind(mSock,AutoClone(this));
}

void _SpaceConnection::onSocketMessage(int event,Socket s,ByteArray data) {
    if(mListener == nullptr) {
        return;
    }

    switch(event) {
        case st(NetEvent)::Message: {
            mParser->pushData(data);
            auto result = mParser->doParse();
            if(result != nullptr && result->size() != 0) {
                ForEveryOne(data,result) {
                    auto msg = mConverter->generateMessage<SpaceMessage>(data);
                    switch(msg->event) {
                        case st(SpaceMessage)::NotifyUpdate:
                            mListener->onDataUpdate(msg->tag,msg->data);
                            break;

                        case st(SpaceMessage)::NotifyRemove:
                            mListener->onDataUpdate(msg->tag,nullptr);
                            break;
                    }
                }
            }
        }

        case st(NetEvent)::Connect:{
            //call onConnect when connected server successfully
            break;
        }

        case st(NetEvent)::Disconnect:{
            close();
            break;
        }

        default:
        break;
    }
}

int _SpaceConnection::close() {
    if(mSock != nullptr) {
        mSocketMonitor->unbind(mSock,false);
        mSock->close();
        mSock = nullptr;
    }
    
    if(mOutput != nullptr) {
        mOutput->close();
        mOutput = nullptr;
    }
    
    if(mInput != nullptr) {
        mInput->close();
        mInput = nullptr;
    }
    return 0;
}

int _SpaceConnection::monitor(ArrayList<String> list) {
    auto message = createSpaceMonitorMessage(list);
    return mOutput->write(mConverter->generatePacket(message));
}

int _SpaceConnection::monitor(String tag) {
    auto message = createSpaceMonitorMessage(tag);
    return mOutput->write(mConverter->generatePacket(message));
}

int _SpaceConnection::unMonitor(ArrayList<String> list) {
    auto message = createSpaceUnMonitorMessage(list);
    return mOutput->write(mConverter->generatePacket(message));
}

int _SpaceConnection::unMonitor(String tag) {
    auto message = createSpaceUnMonitorMessage(tag);
    return mOutput->write(mConverter->generatePacket(message));
}

int _SpaceConnection::remove(String tag) {
    auto message = createSpaceRemoveMessage(tag,nullptr);
    return mOutput->write(mConverter->generatePacket(message));
}

_SpaceConnection::~_SpaceConnection() {
    close();
    if(mSocketMonitor != nullptr) {
        mSocketMonitor->close();
        mSocketMonitor = nullptr;
    }
}

}
