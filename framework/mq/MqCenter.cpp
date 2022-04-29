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
    //printf("MqWorker enqueueMessage start\n");
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
        //printf("MqWorker run start,get a msg type is %d\n",msg->getType());
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
        //printf("msg type is %x \n",msg->getType());
        
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
    printf("publish msg \n");
    auto outputStreams = center->getOutputStreams(msg->getChannel());
    if(outputStreams == nullptr || outputStreams->size() == 0) {
        return;
    }

    //printf("MqWorker run start,public message trace1\n");
    auto iterator = outputStreams->getIterator();
    while(iterator->hasValue()) {
        auto output = iterator->getValue();
        //printf("MqWorker run start,public message trace2\n");
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
    printf("publish one shot!!! \n");
    bool isSendSuccess = true;
    auto outputStreams = center->getOutputStreams(msg->getChannel());
    if(outputStreams == nullptr || outputStreams->size() == 0) {
        if(!msg->isPersist()) {
            printf("publish one shot,but no subscriber \n");
            center->mUndeliveredComp->newMessage(msg->getChannel(),msg->mSerializableData);
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

        printf("publish one shot trace1!!! \n");
        //if(msg->isAcknowledge()) {
        //    String token = center->mUndeliveredComp->newMessage(msg->getChannel(),msg->mSerializableData);
        //    msg->setToken(token);
        //    //post to timer
        //}

        printf("publish one shot trace2,size is %d!!!,current is %d \n",outputStreams->size(),current);
        while(current < outputStreams->size()) {
            //printf("publish one shot trace3 \n");
            if(outputStreams->get(current)->write(msg->mSerializableData) <= 0) {
                //printf("publish one shot trace4 \n");
                outputStreams->removeAt(current);
                isSendSuccess = false;
                current++;
                continue;
            } else {
                //printf("publish one shot trace5 \n");
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
                center->mUndeliveredComp->newMessage(msg->getChannel(),msg->mSerializableData);
            }
        }
    }

}

void _MqWorker::processSubscribe(MqMessage msg) {
    printf("process subscribe start \n");
    if(msg->mSocket->getFileDescriptor() == nullptr) {
        printf("process subscribe file descriptor is nullptr \n");
    } else {
        printf("process subscribe file descriptor is not nullptr \n");
    }

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
        ByteArray persistData;
        String token;
        FetchRet(token,persistData) = comp->take(msg->getChannel());
        printf("subscribe trace1 \n");
        if(persistData != nullptr) {
            ByteArray convertData = createByteArray(persistData->toValue() + sizeof(int),
                                                    persistData->size() - sizeof(int),
                                                    true);

            MqMessage persistMsg = DeSerialize<MqMessage>(convertData);
            printf("subscribe trace1_1 \n");
            if(msg->mSocket->getOutputStream()->write(persistData) > 0) {
                printf("subscribe trace2,token is %s \n",token->toChars());
                comp->remove(persistMsg->getChannel(),token);
                if(persistMsg->isAcknowledge()) {
                    printf("setAcknoledgeTimer \n");
                    setAcknowledgeTimer(persistMsg);
                } 
                
                decreaseCount(msg->getChannel());
                continue;
            }

            //TODO
        }

        break;
    }
    printf("subscribe trace2 \n");
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
        printf("msg timeout,msg type is %d \n",msg->getType());
        int retryTimes = msg->getRetryTimes();
        MqPersistentComponent comp = ((center->mPersistentComp == nullptr)?
        Cast<MqPersistentComponent>(center->mUndeliveredComp):center->mPersistentComp);
        printf("retryTimes is %d,center->mRetryTimes is %d \n",retryTimes,center->mRetryTimes);
        if(retryTimes >= center->mRetryTimes) {
            decreaseCount(msg->getChannel()); 
        } else {
            //resend this action
            msg->setRetryTimes(retryTimes + 1);
            //msg maybe get from db,mSerializableData may be null,reSerializable again
            ByteArray serialdata = Serialize(msg);
            ByteArray savedata = createByteArray(serialdata->size() + sizeof(int));
            ByteArrayWriter writer = createByteArrayWriter(savedata);
            writer->writeInt(serialdata->size());
            writer->writeByteArray(serialdata);
            msg->mSerializableData = savedata;

            comp->newMessage(msg->getChannel(),msg->mSerializableData);
            printf("resend this data,interval is %d \n",center->mRetryInterval);
            center->mTimer->schedule(center->mRetryInterval,[this](MqMessage msg){
                printf("resend data!!!! \n");
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
    printf("MqCenter create start \n");
    sock = createSocketBuilder()->setAddress(mAddrss)->newServerSocket();
    int ret = sock->bind();
    printf("MqCenter create,ret is %d,error is %s \n",ret,CurrentError);
    monitor->bind(sock,AutoClone(this));
    printf("MqCenter create end \n");
}

void _MqCenter::onSocketMessage(int event,Socket sock,ByteArray data) {
    switch(event) {
        case st(NetEvent)::Message:
            //printf("mq center onSocket message,data size is %d \n",data->size());
            mBuffer->push(data);
            
            while(1) {
                int availableDataSize = mBuffer->getAvailDataSize();
                //printf("mq mCurrentMsgLen is %d,availableDataSize is %d \n",mCurrentMsgLen,availableDataSize);
                if(mCurrentMsgLen != 0) {
                    if(mCurrentMsgLen <= availableDataSize) {
                        mReader->move(mCurrentMsgLen);
                        ByteArray data = mReader->pop();
                        //printf("mq dispatch message \n");
                        dispatchMessage(sock,data);
                        mCurrentMsgLen = 0;
                        continue;
                    } else {
                        break;
                    }
                } else {
                    if(mReader->read<int>(mCurrentMsgLen) == st(ByteRingArrayReader)::Continue) {
                        //printf("mCurrentMsgLen is %d \n",mCurrentMsgLen);
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
            //printf("i get a msg worker \n");
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

    printf("center close!!! \n");
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
        printf("close monitor \n");
        monitor->close();
        monitor = nullptr;
    }
    printf("center trace1 close!!! \n");

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
    printf("~~~~~mq center~~~~~\n");
    this->close();
}

}
