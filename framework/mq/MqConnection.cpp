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
    sock = createSocketBuilder()->setAddress(mAddress)->newSocket();
    if(sock->connect() < 0) {
        return -1;
    }
    mInput = sock->getInputStream();
    mOutput = sock->getOutputStream();
    auto conn = AutoClone(this);
    return monitor->bind(sock,conn);
}

int _MqConnection::subscribe(String channel,MqConnectionListener listener) {
    {
        AutoLock l(mMutex);
        auto list = mListeners->get(channel);
        if(list == nullptr) {
            list = createArrayList<MqConnectionListener>();
            mListeners->put(channel,list);
        }
        printf("subscribe channel is %s,mListeners[%lx] size is %d,this is %lx \n",channel->toChars(),mListeners.get_pointer(),mListeners->size(),this);
        list->add(listener);
    }

    MqMessage msg = createMqMessage(channel,nullptr,st(MqMessage)::Subscribe);
    return mOutput->write(msg->generatePacket());
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
                        MqMessage msg = createMqMessage();
                        msg->deserialize(data);
                        String channel = msg->getChannel();
                        {
                            AutoLock l(mMutex);
                            auto list = mListeners->get(channel);
                            if(list != nullptr) {
                                auto iterator = list->getIterator();
                                while(iterator->hasValue()) {
                                    auto listener = iterator->getValue();
                                    if(listener->onEvent(channel,msg->getData()) && msg->isAcknowledge()) {
                                        msg->setFlags(st(MqMessage)::MessageAck);
                                        mOutput->write(msg->generatePacket());
                                    }
                                    iterator->next();
                                }
                            } else {
                                printf("list is nullptr,channel is %s,mListeners[%lx] size is %d,this is %lx,pid is %d \n",channel->toChars(),mListeners.get_pointer(),mListeners->size(),this,st(System)::myPid());
                            }
                        }
                        mCurrentMsgLen = 0;
                        continue;
                    } else {
                        break;
                    }
                } else {
                    if(mReader->read<int>(mCurrentMsgLen) == st(ByteRingArrayReader)::Continue) {
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
    return 0;
}

_MqConnection::~_MqConnection() {
    close();
}



}
