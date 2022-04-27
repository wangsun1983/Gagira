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
    mQueueProcessorIndexs = createHashMap<String,Integer>();
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
        bool isSendSuccess = true;
        MqMessage msg = actions->takeFirst();
        printf("MqWorker run start,get a msg type is %d\n",msg->getType());
        ArrayList<OutputStream> outputStreams = center->getOutputStreams(msg->getChannel());
        
        String token = nullptr;
        if(msg->isPersist()) {
            if(outputStreams == nullptr) {
                center->mPersistentComp->newChannel(msg->getChannel());
            }

            if(msg->getType() != st(MqMessage)::Subscribe) {
                token = center->mPersistentComp->newMessage(msg->getChannel(),
                                                            msg->mSerializableData);
                msg->setToken(token);
            }
        }
        printf("msg type is %x \n",msg->getType());
        
        if((msg->getType() & st(MqMessage)::MessageAck) != 0) {
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
        } else if((msg->getType() & st(MqMessage)::Subscribe) != 0) {
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
                if(persistData != nullptr && msg->mSocket->getOutputStream()->write(persistData) > 0) {
                    printf("subscribe trace2,token is %s \n",token->toChars());
                    comp->remove(msg->getChannel(),token);
                } else {
                    break;
                }
            }
        } else if((msg->getType() & st(MqMessage)::PublishOneShot) != 0) {
            printf("publish one shot!!! \n");
            if(outputStreams == nullptr || outputStreams->size() == 0) {
                isSendSuccess = false;
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
                if(msg->isAcknowledge()) {
                    String token = center->mUndeliveredComp->newMessage(msg->getChannel(),msg->mSerializableData);
                    msg->setToken(token);
                    //post to timer
                }
                printf("publish one shot trace2,size is %d!!!,current is %d \n",outputStreams->size(),current);
                while(current < outputStreams->size()) {
                    printf("publish one shot trace3 \n");
                    if(outputStreams->get(current)->write(msg->mSerializableData) <= 0) {
                        printf("publish one shot trace4 \n");
                        outputStreams->removeAt(current);
                        isSendSuccess = false;
                        current++;
                        continue;
                    } else {
                        printf("publish one shot trace5 \n");
                        isSendSuccess = true;
                        current++;
                        break;
                    }
                }

                if(isSendSuccess && msg->isAcknowledge()) {
                    isSendSuccess = false;
                    Future future = center->mTimer->schedule(center->mAckTimeout,[this,msg](){
                        printf("msg timeout,msg type is %d,msg token is %s \n",msg->getType(),msg->getToken()->toChars());
                        MqPersistentComponent comp = ((center->mPersistentComp == nullptr)?
                        Cast<MqPersistentComponent>(center->mUndeliveredComp):center->mPersistentComp);
                        if(comp->remove(msg->getChannel(),msg->getToken())== 0) {
                            //TODO(move to next channel???);
                            printf("msg timeout,remove it!!!!\n");
                        }

                        //reduce count
                        {
                            AutoLock l(mMutex);
                            Integer v = mChannelCounts->get(msg->getChannel());
                            if(v->toValue() == 1) {
                                center->removeWorkerRecord(msg->getChannel());
                            } else {
                                v->update(v->toValue() - 1);
                            }
                        }
                    });

                    AutoLock l(center->mMutex);
                    center->mAckTimerFuture->put(msg->getToken(),future);
                }
                
                index->update(current);
            }

            //wangsl
            if(!msg->isPersist() & !isSendSuccess && !msg->isAcknowledge()) {
                printf("MqCenter add msg for send fail \n");
                center->mUndeliveredComp->newMessage(msg->getChannel(),msg->mSerializableData);
            }
            //wangsl
        } else if((msg->getType() & st(MqMessage)::Publish) != 0) {
            if(outputStreams == nullptr || outputStreams->size() == 0) {
                isSendSuccess = false;
                break;
            }

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

        //wangsl
        if(msg->isPersist() && isSendSuccess && !msg->isAcknowledge()) {
            center->mPersistentComp->remove(msg->getChannel(),token);
        }
        //wangsl
        if(isSendSuccess) {
            AutoLock l(mMutex);
            Integer v = mChannelCounts->get(msg->getChannel());
            if(v->toValue() == 1) {
                center->removeWorkerRecord(msg->getChannel());
            } else {
                v->update(v->toValue() - 1);
            }
        }
    }
}

int _MqWorker::size() {
    return actions->size();
}

//------------MqCenter-------------
_MqCenter::_MqCenter(String s,int workers,int buffsize,MqPersistentComponent c,int acktimeout) {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        monitor = createSocketMonitor();
    });

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

    mTimer = createThreadScheduledPoolExecutor();
    //create server socket
    sock = createSocketBuilder()->setAddress(mAddrss)->newServerSocket();
    sock->bind();
    monitor->bind(sock,AutoClone(this));
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

}
