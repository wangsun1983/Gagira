#include <mutex>

#include "MqConnection.hpp"
#include "HttpUrl.hpp"
#include "SocketBuilder.hpp"
#include "MqMessage.hpp"
#include "ByteArrayWriter.hpp"
#include "NetEvent.hpp"

using namespace obotcha;

namespace gagira {

SocketMonitor _MqConnection::monitor = nullptr;

_MqConnection::_MqConnection(String s) {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        monitor = createSocketMonitor();
    });

    HttpUrl url = createHttpUrl(s);
    mAddress = url->getInetAddress()->get(0);

    mListeners = createHashMap<String,ArrayList<MqConnectionListener>>();
    mMutex = createMutex();

    mBuffer = createByteRingArray(1024*4);
    mReader = createByteRingArrayReader(mBuffer);
    mCurrentMsgLen = 0;
}

int _MqConnection::connect() {
    printf("MqConnection connect \n");
    sock = createSocketBuilder()->setAddress(mAddress)->newSocket();
    int ret = sock->connect();
    if(ret < 0) {
        printf("connect fail,reason is %s \n",CurrentError);
        return ret;
    }
    printf("MqConnection trace1 \n");
    mInput = sock->getInputStream();
    mOutput = sock->getOutputStream();
    printf("MqConnection trace2 \n");
    monitor->bind(sock,AutoClone(this));
    return 0;
}

int _MqConnection::subscribe(String channel,MqConnectionListener listener) {
    {
        AutoLock l(mMutex);
        auto list = mListeners->get(channel);
        if(list == nullptr) {
            list = createArrayList<MqConnectionListener>();
            mListeners->put(channel,list);
        }
        list->add(listener);
    }
    
    MqMessage msg = createMqMessage(channel,nullptr,st(MqMessage)::Subscribe);
    return mOutput->write(msg->toByteArray());
}

void _MqConnection::onSocketMessage(int event,Socket s,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Message: {
            mBuffer->push(data);
            
            while(1) {
                int availableDataSize = mBuffer->getAvailDataSize();
                //printf("mq mCurrentMsgLen is %d,availableDataSize is %d \n",mCurrentMsgLen,availableDataSize);
                if(mCurrentMsgLen != 0) {
                    if(mCurrentMsgLen <= availableDataSize) {
                        mReader->move(mCurrentMsgLen);
                        ByteArray data = mReader->pop();
                        MqMessage msg = createMqMessage();
                        msg->deserialize(data);
                        String channel = msg->getChannel();
                        {
                            AutoLock l(mMutex);
                            auto list = mListeners->get(channel);
                            auto iterator = list->getIterator();
                            while(iterator->hasValue()) {
                                auto listener = iterator->getValue();
                                //printf("mqconnection on socket message trace1 \n");
                                if(listener->onEvent(channel,msg->getData()) && msg->isAcknowledge()) {
                                    msg->setFlags(st(MqMessage)::MessageAck);
                                    mOutput->write(msg->toByteArray());
                                } else {
                                    //printf("mqconnection on socket message trace2 \n");
                                }
                                iterator->next();
                            }
                        }
                        mCurrentMsgLen = 0;
                        continue;
                    } else {
                        break;
                    }
                } else {
                    if(mReader->read<int>(mCurrentMsgLen) == st(ByteRingArrayReader)::Continue) {
                        //printf("mCurrentMsgLen is %d \n",mCurrentMsgLen);
                        //pop size content
                        mReader->pop();
                        continue;
                    } else {
                        break;
                    }
                }
            }
        }
        break;
    }    
}

int _MqConnection::close() {
    printf("mqconnection close()!!! \n");
    if(mOutput != nullptr) {
        mOutput->close();
        mOutput = nullptr;
    }

    if(mInput != nullptr) {
        mInput->close();
        mInput = nullptr;
    }

    if(sock != nullptr) {
        monitor->remove(sock,false);
        sock->close();
        sock = nullptr;
    }
    printf("mqconnection close() trace1 !!! \n");
    return 0;
}

_MqConnection::~_MqConnection() {
    close();
}



}
