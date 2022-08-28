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
#include "InitializeException.hpp"

using namespace obotcha;

namespace gagira {

//------------MqStreamGroup-------
_MqStreamGroup::_MqStreamGroup() {
    mReadWriteLock = createReadWriteLock();
    mRdLock = mReadWriteLock->getReadLock();
    mWrLock = mReadWriteLock->getWriteLock();
    streams = createArrayList<OutputStream>();
    mRand = createRandom();
}

int _MqStreamGroup::doSubscribe(MqMessage msg) {
    AutoLock l(mWrLock);
    streams->add(msg->mSocket->getOutputStream());
    return 0;
}

int _MqStreamGroup::doOneshot(MqMessage msg) {
    AutoLock l(mRdLock);
    int random = mRand->nextInt();

    if(streams == nullptr || streams->size() == 0) {
        return  -1;
    }

    return streams->get(random%streams->size())->write(msg->mSerializableData);
}

int _MqStreamGroup::doPublish(MqMessage msg) {
    ArrayList<OutputStream> errStreams = nullptr;
    {
        AutoLock l(mRdLock);
        if(streams->size() == 0) {
            return -1;
        }
        auto iterator = streams->getIterator();
        while(iterator->hasValue()) {
            auto output = iterator->getValue();
            if(output->write(msg->mSerializableData) <= 0) {
                LOG(ERROR)<<"MqCenter send fail,this connection may be closed";
                if(errStreams == nullptr) {
                    errStreams = createArrayList<OutputStream>();
                }
                errStreams->add(output);
                continue;
            }
            iterator->next();
        }
    }

    if(errStreams != nullptr) {
        AutoLock l(mWrLock);
        streams->removeAll(errStreams);
    }

    return 0;
}

int _MqStreamGroup::doAck(MqMessage msg) {
    return 0;
}

//------------MqWorker-------------
_MqWorker::_MqWorker(_MqCenter *c) {
    center = c;
    actions = createBlockingLinkedList<MqMessage>();
}

_MqWorker::~_MqWorker() {
    close();
}

void _MqWorker::close() {
    actions->destroy();
}

void _MqWorker::enqueueMessage(MqMessage a) {
    actions->putLast(a);
}

void _MqWorker::run() {
    while(1) {
        MqMessage msg = actions->takeFirst();
        if(msg == nullptr) {
            break;
        }

        String token = nullptr;
        if(msg->isPersist()) {
            if((msg->getType()&st(MqMessage)::Subscribe) != 0) {
                token = center->mPersistence->onNewMessage(msg->getChannel(),
                                                            msg->mSerializableData,
                                                            msg->getFlags());
                msg->setToken(token);
            }
        }
        
        if((msg->getType() & st(MqMessage)::MessageAck) != 0) {
            center->processAck(msg);
        } else if((msg->getType() & st(MqMessage)::Subscribe) != 0) {
            center->processSubscribe(msg);
        } else if((msg->getType() & st(MqMessage)::PublishOneShot) != 0) {
            center->processOneshot(msg);
        } else if((msg->getType() & st(MqMessage)::Publish) != 0) {
            center->processPublish(msg);
        }
    }
}

int _MqWorker::size() {
    return actions->size();
}

//------------MqCenter-------------
_MqCenter::_MqCenter(String s,int workers,int buffsize,MqPersistenceInterface c,int acktimeout,int retryTimes,int retryintervals) {

    monitor = createSocketMonitor();

    mPersistence = c;

    HttpUrl url = createHttpUrl(s);
    mAddrss = url->getInetAddress()->get(0);

    mStreams = createHashMap<String,MqStreamGroup>();
    mStreamRwLock = createReadWriteLock();
    mStreamReadLock = mStreamRwLock->getReadLock();
    mStreamWriteLock = mStreamRwLock->getWriteLock();

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
    if(sock->bind() < 0) {
        Trigger(InitializeException,"MqCenter socket bind failed");
    }

    if(monitor->bind(sock,AutoClone(this)) < 0) {
        Trigger(InitializeException,"MqCenter monitor bind failed");
    }
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
        AutoLock l(mStreamWriteLock);
        mStreams->clear();
    }

    {
        auto iterator = mWorkers->getIterator();
        while(iterator->hasValue()) {
            auto worker = iterator->getValue();
            worker->close();
            worker->join();
            iterator->next();
        }

        AutoLock l(mWorkerWriteLock);
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

//process function
int _MqCenter::processAck(MqMessage msg) {
    if(mPersistence != nullptr) {
        mPersistence->onRemove(msg->getChannel(),msg->getToken());
    }

    Future future = nullptr;

    {
        AutoLock l(mMutex);
        future = mAckTimerFuture->get(msg->getToken());
        mAckTimerFuture->remove(msg->getToken());
    }

    if(future != nullptr) {
        future->cancel();
    }
    return 0;
}

int _MqCenter::processPublish(MqMessage msg) {
    {
        AutoLock l(mStreamReadLock);
        MqStreamGroup group = mStreams->get(msg->channel);
        if(group != nullptr) {
            group->doPublish(msg);
        } else {
            if(mPersistence != nullptr) {
               mPersistence->onFail(msg->getChannel(),msg->toByteArray(),msg->getFlags());
            }
            return -1;
        }
    }

    if(msg->isPersist() && !msg->isAcknowledge() && mPersistence != nullptr) {
        mPersistence->onRemove(msg->getChannel(),msg->getToken());
    }

    return 0;
}

int _MqCenter::processOneshot(MqMessage msg) {
    bool isSendSuccess = true;
    {
        AutoLock l(mStreamReadLock);
        MqStreamGroup group = mStreams->get(msg->channel);
        if(group != nullptr) {
            isSendSuccess = (group->doOneshot(msg) == 0);
        } else {
            if(mPersistence != nullptr) {
               mPersistence->onFail(msg->getChannel(),msg->toByteArray(),msg->getFlags());
            }
            return -1;
        }
    }

    if(isSendSuccess) {
        if(msg->isPersist()) {
            mPersistence->onRemove(msg->getChannel(),msg->getToken());
        }

        if(msg->isAcknowledge()) {
            setAcknowledgeTimer(msg);
        }
    } else {
        if(mPersistence != nullptr) {
            mPersistence->onFail(msg->getChannel(),msg->toByteArray(),msg->getFlags());
        }
    }
    
    return 0;
}

int _MqCenter::processSubscribe(MqMessage msg) {
    MqStreamGroup group = nullptr;
    {
        AutoLock l(mStreamWriteLock);
        group = mStreams->get(msg->channel);
        if(group == nullptr) {
            group = createMqStreamGroup();
            mStreams->put(msg->channel,group);
        }
    }

    group->doSubscribe(msg);

    //check whether there is sticky message in hashmap;
    while(1) {
        ByteArray persistData = nullptr;
        String token = nullptr;
        int flags = -1;
        FetchRet(token,persistData,flags) = mPersistence->onTakeFailMessage(msg->getChannel());//comp->onTakeMessage(msg->getChannel());
        if(persistData != nullptr) {
            ByteArray convertData = createByteArray(persistData->toValue() + sizeof(int),
                                                    persistData->size() - sizeof(int),
                                                    true);

            MqMessage persistMsg = DeSerialize<MqMessage>(convertData);
        
            if(persistData != nullptr && msg->mSocket->getOutputStream()->write(persistData) > 0) {
                mPersistence->onRemove(persistMsg->getChannel(),token);
                if(persistMsg->isAcknowledge()) {
                    setAcknowledgeTimer(persistMsg);
                }
                continue;
            }
        }

        break;
    }

    return 0;
}

int _MqCenter::setAcknowledgeTimer(MqMessage msg) {
    Future future = mTimer->schedule(mAckTimeout,[this](MqMessage msg){
        int retryTimes = msg->getRetryTimes();
        if(retryTimes < mRetryTimes) {
            //resend this action
            msg->setRetryTimes(retryTimes + 1);
            //msg maybe get from db,mSerializableData may be null,reSerializable again
            ByteArray serialdata = Serialize(msg);
            ByteArray savedata = createByteArray(serialdata->size() + sizeof(int));
            ByteArrayWriter writer = createByteArrayWriter(savedata);
            writer->write<int>(serialdata->size());
            writer->write(serialdata);
            msg->mSerializableData = savedata;

            mPersistence->onNewMessage(msg->getChannel(),msg->mSerializableData,msg->getFlags());
            mTimer->schedule(mRetryInterval,[this](MqMessage msg){
                dispatchMessage(msg->mSocket,msg->mSerializableData);
            },msg);
        }
    },msg);

    AutoLock l(mMutex);
    mAckTimerFuture->put(msg->getToken(),future);
    return 0;
}


_MqCenter::~_MqCenter() {
    this->close();
}

}
