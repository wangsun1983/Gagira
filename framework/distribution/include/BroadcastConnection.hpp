#ifndef __GAGRIA_BROADCAST_CONNECTION_HPP__
#define __GAGRIA_BROADCAST_CONNECTION_HPP__

#include "String.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "SocketMonitor.hpp"
#include "BroadcastMessage.hpp"
#include "Mutex.hpp"
#include "ByteRingArray.hpp"
#include "ByteRingArrayReader.hpp"
#include "DistributeMessageParser.hpp"
#include "DistributeMessageConverter.hpp"
#include "ReadWriteLock.hpp"
#include "System.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(BroadcastConnectionListener) {
public:
    enum MessageResponse {
        AutoAck = 0,
        NoAck,
    };

    virtual int onMessage(String channel,ByteArray data) = 0;
    virtual void onDisconnect() = 0;
    virtual void onConnect() = 0;
    virtual void onDetach(String channel) = 0;
    virtual void onSustain(int code,String msg) = 0;
};

template<typename T>
class _connection_helper {
public:
    _connection_helper(T t) {
        this->t = t;
    }

    ByteArray toData() {
        return t->serialize();
    }
public:
    T t;
};

template<>
class _connection_helper<String> {
public:
    _connection_helper(String t) {
        this->t = t;
    }

    ByteArray toData() {
        return t->toByteArray();
    }
public:
    String t;
};

template<>
class _connection_helper<ByteArray> {
public:
    _connection_helper(ByteArray t) {
        this->t = t;
    }

    ByteArray toData() {
        return t;
    }
public:
    ByteArray t;
};

DECLARE_CLASS(BroadcastMessageParam) {
public:
    _BroadcastMessageParam();
    _BroadcastMessageParam* setFlags(uint32_t);
    _BroadcastMessageParam* setTTL(uint32_t);
    _BroadcastMessageParam* setDelayInterval(uint32_t);

    uint32_t getFlags();
    uint32_t getTTL();
    uint32_t getDelayInterval();

    sp<_BroadcastMessageParam> build();
private:
    uint32_t mFlags;
    uint32_t mTTL;
    uint32_t mDelayInterval;
};

DECLARE_CLASS(BroadcastConnection) IMPLEMENTS(SocketListener) {
public:
    _BroadcastConnection(String,BroadcastConnectionListener l = nullptr);
    ~_BroadcastConnection();
    int connect();
    int close();

    template<typename T>
    bool publishMessage(String channel,T obj,BroadcastMessageParam param = nullptr) {
        ByteArray data = _connection_helper<T>(obj).toData();
        uint32_t flags = (param == nullptr)?0:param->getFlags();
        
        BroadcastMessage msg = createBroadcastMessage(channel,data,flags|st(BroadcastMessage)::Publish);
        if(param != nullptr) {
            msg->setTTL(param->getTTL());
            if(param->getDelayInterval() != 0) {
                msg->setPublishTime(st(System)::currentTimeMillis() + param->getDelayInterval());
            }
        }
        return sendMessage(msg);
    }

    template<typename T>
    bool publishStickMessage(String channel,T obj,BroadcastMessageParam param = nullptr) {
        if(param == nullptr) {
            param = createBroadcastMessageParam();
        }
        param->setFlags(param->getFlags()|st(BroadcastMessage)::Publish|st(BroadcastMessage)::StickFlag);
        return publishMessage(channel,obj,param);
    }

    bool subscribeChannel(String channel);
    bool unSubscribeChannel(String channel);
    bool publisAckMessage(String channel,String token);
    bool postBackMessage(ByteArray data,uint32_t flags = st(BroadcastMessage)::StartFalg);
    bool subscribePersistenceChannel();
    bool subscribeDLQChannel();

private:
    bool sendMessage(BroadcastMessage);
    InetAddress mAddress;
    Socket mSock;
    InputStream mInput;
    OutputStream mOutput;
    DistributeMessageConverter mConverter;

    DistributeMessageParser mParser;
    BroadcastConnectionListener mListener;

    SocketMonitor mSocketMonitor;
    void onSocketMessage(int,Socket,ByteArray);

    ReadWriteLock mStatusReadWriteLock;
    ReadLock mStatusReadLock;
    WriteLock mStatusWriteLock;
    int mIsConnected;
};

}
#endif
