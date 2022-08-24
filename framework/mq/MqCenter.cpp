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
#include "ByteArrayWriter.hpp"

using namespace obotcha;

namespace gagira {

//------------MqWorker-------------
_MqWorker::_MqWorker(_MqCenter *c) {
    center = c;
    actions = createBlockingLinkedList<MqMessage>();
    mChannelCounts = createHashMap<String,Integer>();
    mQueueProcessorIndexs = createHashMap<String,Integer>();
    mMutex = createMutex();
    isRunning = true;
}

_MqWorker::~_MqWorker() {
    close();
}

void _MqWorker::close() {
    actions->destroy();
}

void _MqWorker::enqueueMessage(MqMessage a) {
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
    while(1) {
        bool isSendSuccess = true;
        MqMessage msg = actions->takeFirst();
        if(msg == nullptr) {
            break;
        }
        ArrayList<OutputStream> outputStreams = center->getOutputStreams(msg->getChannel());

        String token = nullptr;
        if(msg->isPersist()) {
            if(outputStreams == nullptr) {
                center->mPersistentComp->newChannel(msg->getChannel());
            }

            if((msg->getType()&st(MqMessage)::Subscribe) != 0) {
                token = center->mPersistentComp->newMessage(msg->getChannel(),
                                                            msg->mSerializableData);
                msg->setToken(token);
            }
        }
        
        if((msg->getType() & st(MqMessage)::MessageAck) != 0) {
            processAck(msg);
        } else if((msg->getType() & st(MqMessage)::Subscribe) != 0) {
            processSubscribe(msg);
        } else if((msg->getType() & st(MqMessage)::PublishOneShot) != 0) {
            processOneshot(msg);
        } else if((msg->getType() & st(MqMessage)::Publish) != 0) {
            processPublish(msg);
        }
    }
}

void _MqWorker::processAck(MqMessage msg) {
    MqPersistentComponent comp = ((center->mPersistentComp == nullptr)?
                Cast<MqPersistentComponent>(center->mUndeliveredComp):center->mPersistentComp);

    comp->remove(msg->getChannel(),msg->getToken());

    Future future = nullptr;

    {
        AutoLock l(center->mMutex);
        future = center->mAckTimerFuture->get(msg->getToken());
        center->mAckTimerFuture->remove(msg->getToken());
    }

    if(future != nullptr) {
        future->cancel();
    }

    decreaseCount(msg->getChannel());
}

void _MqWorker::processPublish(MqMessage msg) {
    auto outputStreams = center->getOutputStreams(msg->getChannel());
    if(outputStreams == nullptr || outputStreams->size() == 0) {
        return;
    }

    auto iterator = outputStreams->getIterator();
    while(iterator->hasValue()) {
        auto output = iterator->getValue();
        if(output->write(msg->mSerializableData) <= 0) {
            LOG(ERROR)<<"MqCenter send fail,this connection may be closed";
            iterator->remove();
            continue;
        }
        iterator->next();
    }

    //wangsl
    if(msg->isPersist() && !msg->isAcknowledge()) {
        center->mPersistentComp->remove(msg->getChannel(),msg->getToken());
    }
    decreaseCount(msg->getChannel());
}

void _MqWorker::processOneshot(MqMessage msg) {
    bool isSendSuccess = true;
    auto outputStreams = center->getOutputStreams(msg->getChannel());
    if(outputStreams == nullptr || outputStreams->size() == 0) {
        if(!msg->isPersist()) {
            center->mUndeliveredComp->newMessage(msg->getChannel(),msg->toByteArray());
        }
    } else {
        Integer index = mQueueProcessorIndexs->get(msg->getChannel());
        if(index == nullptr) {
            index = createInteger(0);
            mQueueProcessorIndexs->put(msg->getChannel(),index);
        }

        int current = index->toValue();
        if(current >= outputStreams->size()) {
            current = 0;
        }

        //if(msg->isAcknowledge()) {
        //    String token = center->mUndeliveredComp->newMessage(msg->getChannel(),msg->mSerializableData);
        //    msg->setToken(token);
        //    //post to timer
        //}

        while(current < outputStreams->size()) {
            if(outputStreams->get(current)->write(msg->mSerializableData) <= 0) {
                outputStreams->removeAt(current);
                isSendSuccess = false;
                current++;
                continue;
            } else {
                isSendSuccess = true;
                current++;
                break;
            }
        }

        index->update(current);

        if(isSendSuccess) {
            if(msg->isPersist()) {
                center->mPersistentComp->remove(msg->getChannel(),msg->getToken());
            }

            if(msg->isAcknowledge()) {
                setAcknowledgeTimer(msg);
            } else {
                decreaseCount(msg->getChannel());
            }
        } else {
            if(!msg->isPersist()) {
                center->mUndeliveredComp->newMessage(msg->getChannel(),msg->toByteArray());
            }
        }
    }

}

void _MqWorker::processSubscribe(MqMessage msg) {
    auto outputStreams = center->getOutputStreams(msg->getChannel());
    if(outputStreams == nullptr) {
        outputStreams = createArrayList<OutputStream>();
        center->setOutputStreams(msg->getChannel(),outputStreams);
    }
    outputStreams->add(msg->mSocket->getOutputStream());

    //check whether there is sticky message in hashmap;
    MqPersistentComponent comp = ((center->mPersistentComp == nullptr)?
                Cast<MqPersistentComponent>(center->mUndeliveredComp):center->mPersistentComp);
    while(1) {
        ByteArray persistData = nullptr;
        String token = nullptr;
        FetchRet(token,persistData) = comp->take(msg->getChannel());
        if(persistData != nullptr) {
            ByteArray convertData = createByteArray(persistData->toValue() + sizeof(int),
                                                    persistData->size() - sizeof(int),
                                                    true);

            MqMessage persistMsg = DeSerialize<MqMessage>(convertData);
        
            if(persistData != nullptr && msg->mSocket->getOutputStream()->write(persistData) > 0) {
                comp->remove(persistMsg->getChannel(),token);
                if(persistMsg->isAcknowledge()) {
                    setAcknowledgeTimer(persistMsg);
                }

                decreaseCount(msg->getChannel());
                continue;
            }

            //TODO
        }

        break;
    }
}

int _MqWorker::size() {
    return actions->size();
}

void _MqWorker::decreaseCount(String channel,int count) {
    AutoLock l(mMutex);
    Integer v = mChannelCounts->get(channel);
    int value = v->toValue();

    if(value != 0) {
        value = (value > count)?(value - count):0;
    }

    v->update(value);
}

void _MqWorker::increaseCount(String channel,int count) {
    AutoLock l(mMutex);
    Integer v = mChannelCounts->get(channel);
    int value = v->toValue();
    v->update(value + count);
}

void _MqWorker::setAcknowledgeTimer(MqMessage msg) {
    Future future = center->mTimer->schedule(center->mAckTimeout,[this](MqMessage msg){
        int retryTimes = msg->getRetryTimes();
        MqPersistentComponent comp = ((center->mPersistentComp == nullptr)?
        Cast<MqPersistentComponent>(center->mUndeliveredComp):center->mPersistentComp);
        if(retryTimes >= center->mRetryTimes) {
            decreaseCount(msg->getChannel());
        } else {
            //resend this action
            msg->setRetryTimes(retryTimes + 1);
            //msg maybe get from db,mSerializableData may be null,reSerializable again
            ByteArray serialdata = Serialize(msg);
            ByteArray savedata = createByteArray(serialdata->size() + sizeof(int));
            ByteArrayWriter writer = createByteArrayWriter(savedata);
            writer->write<int>(serialdata->size());
            writer->write(serialdata);
            msg->mSerializableData = savedata;

            comp->newMessage(msg->getChannel(),msg->mSerializableData);
            center->mTimer->schedule(center->mRetryInterval,[this](MqMessage msg){
                center->dispatchMessage(msg->mSocket,msg->mSerializableData);
            },msg);
        }
    },msg);

    AutoLock l(center->mMutex);
    center->mAckTimerFuture->put(msg->getToken(),future);
}

//------------MqCenter-------------
_MqCenter::_MqCenter(String s,int workers,int buffsize,MqPersistentComponent c,int acktimeout,int retryTimes,int retryintervals) {

    monitor = createSocketMonitor();

    mUndeliveredComp = createMqUndeliveredComponent();

    if(c != nullptr) {
        mPersistentComp = c;
    } else {
        mPersistentComp = mUndeliveredComp;
    }

    HttpUrl url = createHttpUrl(s);
    mAddrss = url->getInetAddress()->get(0);

    mOutputStreams = createHashMap<String,ArrayList<OutputStream>>();
    mReadWriteLock = createReadWriteLock();
    mReadLock = mReadWriteLock->getReadLock();
    mWriteLock = mReadWriteLock->getWriteLock();

    mBuffer = createByteRingArray(buffsize);
    mReader = createByteRingArrayReader(mBuffer);
    mCurrentMsgLen = 0;

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

    mMutex = createMutex();
    waitExit = createCondition();

    mAckTimeout = acktimeout;

    mAckTimerFuture = createHashMap<String,Future>();

    mRetryInterval = retryintervals;
    mRetryTimes = retryTimes;

    mTimer = createThreadScheduledPoolExecutor();
    //create server socket
    sock = createSocketBuilder()->setAddress(mAddrss)->newServerSocket();
    int ret = sock->bind();
    monitor->bind(sock,AutoClone(this));
}

void _MqCenter::onSocketMessage(int event,Socket sock,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Message:
            mBuffer->push(data);

            while(1) {
                int availableDataSize = mBuffer->getAvailDataSize();
                if(mCurrentMsgLen != 0) {
                    if(mCurrentMsgLen <= availableDataSize) {
                        mReader->move(mCurrentMsgLen);
                        ByteArray data = mReader->pop();
                        dispatchMessage(sock,data);
                        mCurrentMsgLen = 0;
                        continue;
                    } else {
                        break;
                    }
                } else {
                    if(mReader->read<int>(mCurrentMsgLen) == st(ByteRingArrayReader)::Continue) {
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
        auto iterator = mWorkers->getIterator();
        while(iterator->hasValue()) {
            auto worker = iterator->getValue();
            worker->close();
            worker->join();
            iterator->next();
        }
        mWorkers->clear();
    }

    {
        this->mTimer->shutdown();
        mTimer->awaitTermination();
    }

    if(sock != nullptr) {
        monitor->remove(sock);
        sock->close();
        sock = nullptr;
    }

    if(monitor != nullptr) {
        monitor->close();
        monitor = nullptr;
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


void _MqCenter::removeWorkerRecord(String channel) {
    AutoLock l(mWorkerWriteLock);
    mMqWorkerMap->remove(channel);
}

_MqCenter::~_MqCenter() {
    this->close();
}

}
