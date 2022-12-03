#include <mutex>

#include "MqConnection.hpp"
#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "MqMessage.hpp"
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

bool _MqConnection::subscribeChannel(String channel) {
    MqMessage msg = createMqMessage(channel,nullptr,st(MqMessage)::Subscribe);
    auto packet = msg->generatePacket();
    return mOutput->write(packet) > 0;
}

bool _MqConnection::unSubscribeChannel(String channel) {
    MqMessage msg = createMqMessage(channel,nullptr,st(MqMessage)::UnSubscribe);
    return mOutput->write(msg->generatePacket()) > 0;
}

void _MqConnection::onSocketMessage(int event,Socket s,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Message: {
            mParser->pushData(data);
            auto result = mParser->doParse();
            if(result != nullptr && result->size() != 0) {
                ForEveryOne(data,result) {
                    auto msg = st(MqMessage)::generateMessage(data);
                    String channel = msg->getChannel();
                    if(mListener != nullptr) {
                        if(msg->getType() == st(MqMessage)::Detach) {
                            mListener->onDetach(channel);
                        } else {
                            int ret = mListener->onMessage(channel,msg->getData());
                            if(msg->isAcknowledge() && ret == st(MqConnectionListener)::AutoAck) {
                                msg->setFlags(st(MqMessage)::Ack);
                                msg->clearData();
                                s->getOutputStream()->write(msg->generatePacket());
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
            close();
            break;
        }

        default:
        break;
    }
}

int _MqConnection::close() {
    if(mSock != nullptr) {
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
