#ifndef __GAGRIA_MAP_CONNECTION_HPP__
#define __GAGRIA_MAP_CONNECTION_HPP__
#include <atomic>

#include "String.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "SocketMonitor.hpp"
#include "Mutex.hpp"
#include "ByteRingArray.hpp"
#include "ByteRingArrayReader.hpp"
#include "DistributeMessageParser.hpp"
#include "DistributeMessageConverter.hpp"
#include "MapMessage.hpp"
#include "ReadWriteLock.hpp"
#include "System.hpp"
#include "SocketBuilder.hpp"
#include "Net.hpp"
#include "Condition.hpp"
#include "Mutex.hpp"
#include "AtomicNumber.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MapResultWaiter) {
public:
    _MapResultWaiter();
    void setResponse(MapMessage resp);
    MapMessage getResponse();
    void wait();
private:
    bool isProcessed;
    Mutex mMutex;
    Condition mCond;
    MapMessage mResponse;
};

DECLARE_CLASS(MapResultWaiterManager) {
public:
    _MapResultWaiterManager();
    void notifyResult(MapMessage result);
    void addWaiter(uint64_t,MapResultWaiter);
    void close();
private:
    ConcurrentHashMap<uint64_t,MapResultWaiter> mWaiters;
};

DECLARE_CLASS(MapConnectionListener) {
public:
    enum Event {
        Create = 0,
        Update,
        Remove
    };
    
    virtual void onConnect() {};
    virtual void onDisconnect() {};
    virtual void onEvent(int event,String tag,ByteArray data) = 0;
};

DECLARE_CLASS(MapConnection) IMPLEMENTS(SocketListener) {
public:
    _MapConnection(String,MapConnectionListener l = nullptr);
    ~_MapConnection();

    template<typename T>
    int update(String tag,T value) {
        mError = 0;
        auto id = getReqId();
        auto waiter = MapResultWaiter::New();
        mWaiterManager->addWaiter(id,waiter);
        auto message = MapUpdateMessage::New(id,tag,value->serialize(),nullptr);
        {
            AutoLock l(mMutex);
            if(mOutput != nullptr) {
                auto ret = mOutput->write(mConverter->generatePacket(message));
                if(ret == 0) {
                    mError = -ENETUNREACH; 
                    return mError;
                }
            } else {
                mError = -ESHUTDOWN;
                return mError;
            }
        }
        waiter->wait();
        mError = -waiter->getResponse()->getResult();
        return mError;
    }

    int remove(String tag);

    int monitor(ArrayList<String>);
    int monitor(String);
    int unMonitor(ArrayList<String>);
    int unMonitor(String);

    template< typename T>
    T get(String tag) {
        mError = 0;

        auto id = getReqId();
        auto message = MapAcquireMessage::New(id,tag);
        auto waiter = MapResultWaiter::New();
        mWaiterManager->addWaiter(id,waiter);
        {
            AutoLock l(mMutex);
            if(mOutput != nullptr) {
                if(mOutput->write(mConverter->generatePacket(message)) == 0) {
                    mError = -ENETUNREACH;
                    return nullptr;
                };
            } else {
                mError = -ESHUTDOWN;
                return nullptr;
            }
        }
        waiter->wait();
        mError = -waiter->getResponse()->getResult();

        if(mError == 0) {
            T ret = AutoCreate<T>();
            ret->deserialize(waiter->getResponse()->getData());
            return ret;
        }

        return nullptr;
    }

    int connect();
    int close();
    int getErr();

private:
    InetAddress mAddress;
    Socket mSock;
    InputStream mInput;
    OutputStream mOutput;
    DistributeMessageConverter mConverter;
    DistributeMessageParser mParser;
    MapConnectionListener mListener;
    MapResultWaiterManager mWaiterManager;
    Mutex mMutex;
    uint64_t mReqId;
    int mError;
    
    SocketMonitor mSocketMonitor;
    void onSocketMessage(st(Net)::Event,Socket,ByteArray);
    int waitForResult(uint64_t mReqId,ByteArray data);
    int getReqId();
};

}
#endif
