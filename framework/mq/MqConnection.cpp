#include <mutex>

#include "MqConnection.hpp"
#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "MqMessage.hpp"
#include "ByteArrayWriter.hpp"
#include "NetEvent.hpp"
#include "InitializeException.hpp"
#include "ForEveryOne.hpp"

using namespace obotcha;

namespace gagira {

_MqConnection::_MqConnection(String s,MqConnectionListener l) {    
    HttpUrl url = createHttpUrl(s);
    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find MqCenter");
    }

    //mMutex = createMutex();
    //mListeners = createHashMap<String,ArrayList<MqConnectionListener>>();
    mListener = l;
    
    mBuffer = createByteRingArray(1024*4);
    mReader = createByteRingArrayReader(mBuffer);
    mSocketMonitor = createSocketMonitor();
    
    mCurrentMsgLen = 0;
}

int _MqConnection::connect() {
    mSock = createSocketBuilder()->setAddress(mAddress)->newSocket();
    if(mSock->connect() < 0) {
        return -1;
    }
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

void _MqConnection::onSocketMessage(int event,Socket s,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Message: {
            mBuffer->push(data);
            while(1) {
                int availableDataSize = mBuffer->getAvailDataSize();
                if(mCurrentMsgLen != 0) {
                    if(mCurrentMsgLen <= availableDataSize) {
                        mReader->move(mCurrentMsgLen);
                        ByteArray data = mReader->pop();
                        auto msg = st(MqMessage)::generateMessage(data);
                        String channel = msg->getChannel();
                        if(mListener != nullptr) {
                            if(msg->isDetach()) {
                                mListener->onDetach(channel);
                            } else {
                                mListener->onMessage(channel,msg->getData());//TODO isNeedAck?
                            }
                        }
                        mCurrentMsgLen = 0;
                        continue;
                    }
                } else if(mReader->read<uint32_t>(mCurrentMsgLen) == st(ByteRingArrayReader)::Continue) {
                    //mReader->pop();
                    continue;
                }
                break;
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
        mSocketMonitor->remove(mSock,false);
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
