#ifndef __GAGRIA_MQ_MESSAGE_HPP__
#define __GAGRIA_MQ_MESSAGE_HPP__

#include "String.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "Reflect.hpp"
#include "Serializable.hpp"
#include "String.hpp"
#include "Serializable.hpp"
#include "UUID.hpp"

using namespace obotcha;

namespace gagira {

class _MqWorker;
class _MqCenter;

DECLARE_CLASS(MqMessage) IMPLEMENTS(Serializable){
public:
    friend class _MqWorker;
    friend class _MqCenter;
    enum MessageType {
        //message type
        ShakeHand = 1<<1,
        Subscribe = 1<<2,
        UnSubscribe = 1<<3,
        Publish = 1<<4,
        PublishOneShot = 1<<5,
        MessageAck = 1<<6,
        MaxMessageType = 1<<7,

        //psersist
        Persistent = 1<<10,
        Acknowledge = 1<<11,
    };
    
    _MqMessage();
    _MqMessage(String channel,ByteArray data,uint32_t flags);
    
    ByteArray toByteArray();

    int getType();

    ByteArray getData();

    String getChannel();

    bool isPersist();
    bool isAcknowledge();

    String getId();

    String getToken();
    void setToken(String);

    void acknowledge();

    void setFlags(uint32_t);

private:
    ByteArray data;
    String channel;
    String token;
    uint32_t flags;

    Socket mSocket;
    ByteArray mSerializableData;

public:
    DECLARE_REFLECT_FIELD(MqMessage,channel,data,token,flags);
};


}
#endif
