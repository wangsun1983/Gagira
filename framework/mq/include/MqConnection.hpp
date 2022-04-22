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

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqConnectionListener) {
public:
    virtual void onEvent(String channel,ByteArray data) = 0;
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

DECLARE_CLASS(MqConnection) IMPLEMENTS(SocketListener) {
public:
    _MqConnection(String);
    int connect();

    template<typename T>
    int publish(String channel,T obj) {
        ByteArray data = _connection_helper<T>(obj).toData();
        MqMessage msg = createMqMessage(st(MqMessage)::Publish,channel,data);
        ByteArray testdata = msg->toByteArray();
        return mOutput->write(msg->toByteArray());
    }
    
    template<typename T>
    int publishStick(String channel,T obj) {
        ByteArray data = _connection_helper<T>(obj).toData();
        MqMessage msg = createMqMessage(st(MqMessage)::PublishStick,channel,data);
        return mOutput->write(msg->toByteArray());
    }

    int subscribe(String channel,MqConnectionListener);

private:
    InetAddress mAddress;
    Socket sock;
    InputStream mInput;
    OutputStream mOutput;

    ByteRingArray mBuffer;
    ByteRingArrayReader mReader;
    int mCurrentMsgLen;

    Mutex mMutex;
    HashMap<String,ArrayList<MqConnectionListener>> mListeners;

    static SocketMonitor monitor;
    void onSocketMessage(int,Socket,ByteArray);
};

}
#endif
