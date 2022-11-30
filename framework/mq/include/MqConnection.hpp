#ifndef __GAGRIA_MQ_CONNECTION_HPP__
#define __GAGRIA_MQ_CONNECTION_HPP__

#include "String.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "SocketMonitor.hpp"
#include "MqMessage.hpp"
#include "Mutex.hpp"
#include "ByteRingArray.hpp"
#include "ByteRingArrayReader.hpp"
#include "MqParser.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqConnectionListener) {
public:
    enum MessageResponse {
        AutoAck = 0,
        NoAck,
    };

    virtual int onMessage(String channel,ByteArray data) = 0;
    virtual void onDisconnect() = 0;
    virtual void onConnect() = 0;
    virtual void onDetach(String channel) = 0;
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

DECLARE_CLASS(MqConnection) IMPLEMENTS(SocketListener) {
public:
    _MqConnection(String,MqConnectionListener l = nullptr);
    ~_MqConnection();
    int connect();
    int close();

    template<typename T>
    bool publish(String channel,T obj,int flags = st(MqMessage)::Publish) {
        ByteArray data = _connection_helper<T>(obj).toData();
        MqMessage msg = createMqMessage(channel,data,flags);
        return mOutput->write(msg->generatePacket()) > 0;
    }

    template<typename T>
    bool stick(String channel,String tag,T obj) {
        ByteArray data = _connection_helper<T>(obj).toData();
        MqMessage msg = createMqMessage(channel,tag,data,st(MqMessage)::Publish|st(MqMessage)::Stick);
        return mOutput->write(msg->generatePacket()) > 0;
    }

    bool unStick(String channel,String tag);
    bool subscribe(String channel);
    bool unSubscribe(String channel);

private:
    InetAddress mAddress;
    Socket mSock;
    InputStream mInput;
    OutputStream mOutput;

    MqParser mParser;
    MqConnectionListener mListener;

    SocketMonitor mSocketMonitor;
    void onSocketMessage(int,Socket,ByteArray);
};

}
#endif
