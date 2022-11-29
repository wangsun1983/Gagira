#include <mutex>

#include "MqConnection.hpp"
#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "MqMessage.hpp"
#include "ByteArrayWriter.hpp"
#include "NetEvent.hpp"
#include "InitializeException.hpp"
#include "ForEveryOne.hpp"
#include "Inspect.hpp"

using namespace obotcha;

namespace gagira {

_MqConnection::_MqConnection(String s,MqConnectionListener l) {
    HttpUrl url = createHttpUrl(s);
    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find MqCenter");
    }

    mListener = l;
    mSocketMonitor = createSocketMonitor();
    mParser = createMqParser(1024*4);
}

int _MqConnection::connect() {
    mSock = createSocketBuilder()->setAddress(mAddress)->newSocket();
    Inspect(mSock->connect() < 0,-1);

    mInput = mSock->getInputStream();
    mOutput = mSock->getOutputStream();

    if(mListener != nullptr) {
        mListener->onConnect();
    }

    return mSocketMonitor->bind(mSock,AutoClone(this));
}

int _MqConnection::subscribe(String channel) {
    MqMessage msg = createMqMessage(channel,nullptr,st(MqMessage)::Subscribe);
    auto packet = msg->generatePacket();
    if(mOutput->write(packet) > 0) {
        return 0;
    }

    return -1;
}

int _MqConnection::unSubscribe(String channel) {
    MqMessage msg = createMqMessage(channel,nullptr,st(MqMessage)::UnSubscribe);
    if(mOutput->write(msg->generatePacket()) > 0) {
        return 0;
    }

    return -1;
}

int _MqConnection::unStick(String channel,String tag) {
    MqMessage msg = createMqMessage(channel,tag,nullptr,st(MqMessage)::UnStick);
    if(mOutput->write(msg->generatePacket()) > 0) {
        return 0;
    }

    return -1;
}

void _MqConnection::onSocketMessage(int event,Socket s,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Message: {
            //mBuffer->push(data);
            mParser->pushData(data);
            auto result = mParser->doParse();
            if(result != nullptr && result->size() != 0) {
                ForEveryOne(data,result) {
                    auto msg = st(MqMessage)::generateMessage(data);
                    String channel = msg->getChannel();
                    if(mListener != nullptr) {
                        if(msg->isDetach()) {
                            mListener->onDetach(channel);
                        } else {
                            int ret = mListener->onMessage(channel,msg->getData());
                            if(msg->isAcknowledge() && ret == 0) {
                                msg->setFlags(st(MqMessage)::MessageAck);
                                msg->clearData();
                                int ret = s->getOutputStream()->write(msg->generatePacket());
                            }
                        }
                    }
                }
            }
        }

        case st(NetEvent)::Connect:{
            //call onConnect when connected server successfully
            break;
        }

        case st(NetEvent)::Disconnect:{
            if(mListener != nullptr) {
                mListener->onDisconnect();
            }
            break;
        }

        default:
        break;
    }
}

int _MqConnection::close() {
    if(mSock != nullptr) {
        //mSocketMonitor->remove(mSock,false);
        mSocketMonitor->unbind(mSock,false);
        mSock->close();
        mSock = nullptr;
    }

    if(mSocketMonitor != nullptr) {
        mSocketMonitor->close();
        mSocketMonitor = nullptr;
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

_MqConnection::~_MqConnection() {
    close();
}



}
