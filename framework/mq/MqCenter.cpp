#include <thread>
#include <atomic>
#include <mutex>

#include "MqCenter.hpp"
#include "HttpUrl.hpp"
#include "ServerSocket.hpp"
#include "SocketBuilder.hpp"
#include "NetEvent.hpp"
#include "MqMessage.hpp"
#include "ByteArrayReader.hpp"

using namespace obotcha;

namespace gagira {

SocketMonitor _MqCenter::monitor = nullptr;

_MqCenter::_MqCenter(String s,int buffsize) {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        monitor = createSocketMonitor();
    });

    HttpUrl url = createHttpUrl(s);
    mAddrss = url->getInetAddress()->get(0);

    //create server socket
    sock = createSocketBuilder()->setAddress(mAddrss)->newServerSocket();
    sock->bind();
    monitor->bind(sock,AutoClone(this));

    mOutputStreams = createHashMap<String,ArrayList<OutputStream>>();
    mReadWriteLock = createReadWriteLock();
    mReadLock = mReadWriteLock->getReadLock();
    mWriteLock = mReadWriteLock->getWriteLock();

    mBuffer = createByteRingArray(buffsize);
    mReader = createByteRingArrayReader(mBuffer);
    mCurrentMsgLen = 0;

    mMutex = createMutex();
    mStickyMessasge = createHashMap<String,ByteArray>();

    waitExit = createCondition();;
}

void _MqCenter::onSocketMessage(int event,Socket sock,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Message:
            printf("mq center onSocket message,data size is %d \n",data->size());
            mBuffer->push(data);
            
            while(1) {
                int availableDataSize = mBuffer->getAvailDataSize();
                printf("mq mCurrentMsgLen is %d,availableDataSize is %d \n",mCurrentMsgLen,availableDataSize);
                if(mCurrentMsgLen != 0) {
                    if(mCurrentMsgLen <= availableDataSize) {
                        mReader->move(mCurrentMsgLen);
                        ByteArray data = mReader->pop();
                        printf("mq dispatch message \n");
                        dispatchMessage(sock,data);
                        mCurrentMsgLen = 0;
                        continue;
                    } else {
                        break;
                    }
                } else {
                    if(mReader->read<int>(mCurrentMsgLen) == st(ByteRingArrayReader)::Continue) {
                        printf("mCurrentMsgLen is %d \n",mCurrentMsgLen);
                        //pop size content
                        //mReader->pop();
                        continue;
                    } else {
                        break;
                    }
                }
            }
        break;
    }
}

void _MqCenter::dispatchMessage(Socket sock,ByteArray data) {
    ByteArray msgData = createByteArray(data->toValue() + sizeof(int),data->size() - sizeof(int),true);
    MqMessage msg = createMqMessage();
    msg->deserialize(msgData);
    String channel = msg->getChannel();
    printf("dispatchMessage msg type is %d,channle is %s \n",msg->getType(),channel->toChars());
    auto list = mOutputStreams->get(channel);
    switch(msg->getType()) {
        case st(MqMessage)::Subscribe: {
            {
                AutoLock l(mWriteLock);
                if(list == nullptr) {
                    list = createArrayList<OutputStream>();
                    mOutputStreams->put(channel,list);
                }
                list->add(sock->getOutputStream());
            }
            //check whether there is sticky message in hashmap;
            {
                AutoLock l(mMutex);
                auto msg = mStickyMessasge->get(channel);
                if(msg != nullptr) {
                    sock->getOutputStream()->write(msg);
                }
            }
        }
        break;

        case st(MqMessage)::PublishStick: {
            AutoLock l(mMutex);
            mStickyMessasge->put(msg->getChannel(),data);
        }

        case st(MqMessage)::Publish: {
            printf("dispatchMessage publish trace,data size is %d\n",data->size());
            AutoLock l(mReadLock);
            if(list != nullptr) {
                auto iterator = list->getIterator();
                while(iterator->hasValue()) {
                    auto output = iterator->getValue();
                    if(output->write(data) <= 0) {
                        LOG(ERROR)<<"MqCenter send fail";
                    }
                    iterator->next();
                }
            }
            printf("dispatchMessage publish finished\n");
        }
        break;
    }
    
}
    

int _MqCenter::close() {
    {
        AutoLock l(mWriteLock);
        mOutputStreams->clear();
    }

    {
        AutoLock l(mMutex);
        mStickyMessasge->clear();
    }

    waitExit->notify();
    return 0;
}

void _MqCenter::waitForExit(long interval) {
    AutoLock l(mMutex);
    waitExit->wait(mMutex,interval);
}

}
