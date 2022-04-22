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

    mBuffer = createByteRingArray(1024);
    mReader = createByteRingArrayReader(mBuffer);
    mCurrentMsgLen = 0;
}

int _MqConnection::connect() {
    sock = createSocketBuilder()->setAddress(mAddress)->newSocket();
    int ret = sock->connect();
    if(ret < 0) {
        return ret;
    }

    mInput = sock->getInputStream();
    mOutput = sock->getOutputStream();

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
    
    MqMessage msg = createMqMessage(st(MqMessage)::Subscribe,channel,nullptr);
    return mOutput->write(msg->toByteArray());
}

void _MqConnection::onSocketMessage(int event,Socket s,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Message: {
            mBuffer->push(data);
            
            while(1) {
                int availableDataSize = mBuffer->getAvailDataSize();
                printf("mq mCurrentMsgLen is %d,availableDataSize is %d \n",mCurrentMsgLen,availableDataSize);
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
                                listener->onEvent(channel,msg->getData());
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
                        printf("mCurrentMsgLen is %d \n",mCurrentMsgLen);
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



}
