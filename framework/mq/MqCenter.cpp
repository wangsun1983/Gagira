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

//------------MqWorker-------------
_MqWorker::_MqWorker(_MqCenter *c) {
    center = c;
    actions = createBlockingLinkedList<MqMessage>();
    mChannelCounts = createHashMap<String,Integer>();
    mMutex = createMutex();
    isRunning = true;
}

void _MqWorker::enqueueMessage(MqMessage a) {
    printf("MqWorker enqueueMessage start\n");
    Integer v = nullptr;
    {
        AutoLock l(mMutex);
        String channel = a->getChannel();
        v = mChannelCounts->get(channel);
        if(v == nullptr) {
            v = createInteger(0);
            mChannelCounts->put(channel,v);
        }
        v->update(v->toValue() + 1);
    }
    actions->putLast(a);
}

void _MqWorker::run() {
    while(isRunning) {
        MqMessage msg = actions->takeFirst();
        printf("MqWorker run start,get a msg type is %d\n",msg->getType());
        ArrayList<OutputStream> outputStreams = center->getOutputStreams(msg->getChannel());
        switch(msg->getType()) {
            case st(MqMessage)::Subscribe: {
                if(outputStreams == nullptr) {
                    outputStreams = createArrayList<OutputStream>();
                    center->setOutputStreams(msg->getChannel(),outputStreams);
                }
                outputStreams->add(msg->mSocket->getOutputStream());

                //check whether there is sticky message in hashmap;
                ByteArray serialData = center->getStickyMessage(msg->getChannel());
                if(msg->mSocket != nullptr && serialData != nullptr) {
                    msg->mSocket->getOutputStream()->write(serialData);
                }
            }
            break;

            case st(MqMessage)::PublishStick:
                if(msg->mSerializableData != nullptr) {
                    center->addStickyMessage(msg->getChannel(),msg->mSerializableData);
                }

            case st(MqMessage)::Publish: {
                printf("MqWorker run start,public message trace1\n");
                auto iterator = outputStreams->getIterator();
                while(iterator->hasValue()) {
                    auto output = iterator->getValue();
                    printf("MqWorker run start,public message trace2\n");
                    if(output->write(msg->mSerializableData) <= 0) {
                        LOG(ERROR)<<"MqCenter send fail,this connection may be closed";
                        iterator->remove();
                        continue;
                    }
                    iterator->next();
                }
            }
        }

        AutoLock l(mMutex);
        Integer v = mChannelCounts->get(msg->getChannel());
        if(v->toValue() == 1) {
            center->removeWorkerRecord(msg->getChannel());
        } else {
            v->update(v->toValue() - 1);
        }
    }
}

int _MqWorker::size() {
    return actions->size();
}

//------------MqCenter-------------
_MqCenter::_MqCenter(String s,int workers,int buffsize) {
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

    mWorkerReadWriteLock = createReadWriteLock();
    mWorkerReadLock = mWorkerReadWriteLock->getReadLock();
    mWorkerWriteLock = mWorkerReadWriteLock->getWriteLock();
    mWorkers = createArrayList<MqWorker>();
    mMqWorkerMap = createHashMap<String,MqWorker>();
    for(int i = 0;i < workers;i++) {
        auto worker = createMqWorker(this);
        worker->start();
        mWorkers->add(worker);
    }

    waitExit = createCondition();
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

        case st(NetEvent)::Disconnect:
        //Do nothing
        break;
    }
}

void _MqCenter::dispatchMessage(Socket sock,ByteArray data) {
    ByteArray msgData = createByteArray(data->toValue() + sizeof(int),data->size() - sizeof(int),true);
    MqMessage msg = createMqMessage();
    msg->deserialize(msgData);
    msg->mSocket = sock;
    msg->mSerializableData = data;

    String channel = msg->getChannel();

    
    //find a worker thread
    {
        AutoLock l(mWorkerReadLock);
        MqWorker worker = mMqWorkerMap->get(channel);
        if(worker != nullptr) {
            printf("i get a msg worker \n");
            worker->enqueueMessage(msg);
            return;
        }
    }

    //find a worker
    int minIndex = 0;
    int minSize = 0;
    for(int i = 0; i < mWorkers->size();i++) {
        auto worker = mWorkers->get(i);
        int size = worker->size();
        if(size == 0) {
            minIndex = i;
            break;
        }

        if(minSize > size) {
            minIndex = i;
            minSize = size;
        }
    }
    
    AutoLock l(mWorkerWriteLock);
    mMqWorkerMap->put(channel,mWorkers->get(minIndex));
    mWorkers->get(minIndex)->enqueueMessage(msg);
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

ArrayList<OutputStream> _MqCenter::getOutputStreams(String channel) {
    AutoLock l(mReadLock);
    return mOutputStreams->get(channel);
}

void _MqCenter::setOutputStreams(String channel,ArrayList<OutputStream> list) {
    AutoLock l(mWriteLock);
    mOutputStreams->put(channel,list);
}

void _MqCenter::addStickyMessage(String channel,ByteArray data) {
    AutoLock l(mMutex);
    mStickyMessasge->put(channel,data);
}

ByteArray _MqCenter::getStickyMessage(String channel) {
    AutoLock l(mMutex);
    return mStickyMessasge->get(channel);
}

void _MqCenter::removeWorkerRecord(String channel) {
    AutoLock l(mWorkerWriteLock);
    mMqWorkerMap->remove(channel);
}

}
