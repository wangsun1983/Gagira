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

/**
 * 
 * MqMessage Serializable Struct
 * {totalsize|size1|data1|size2|data2....}
 * 
 */
DECLARE_CLASS(MqMessage) IMPLEMENTS(Serializable){
public:
    friend class _MqWorker;
    friend class _MqCenter;
    friend class _MqStreamGroup;

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
    
    _MqMessage(String channel,ByteArray data,uint32_t types);

    ByteArray generatePacket();
    
    static MqMessage generateMessage(ByteArray); 

    ByteArray getData();

    String getChannel();

    bool isPersist();

    bool isAcknowledge();

    bool isShakeHand();

    bool isSubscribe();

    bool isUnSubscribe();

    bool isPublish();

    bool isPublishOneShot();
    
    bool isAck();

    String getToken();

    void setToken(String);

    void setFlags(uint32_t);

    uint32_t getFlags();

    int getRetryTimes();

    void setRetryTimes(int);

private:
    ByteArray data;
    String channel;
    String token;
    uint32_t flags;

    Socket mSocket;
    ByteArray mPacketData;
    int retryTimes;

public:
    DECLARE_REFLECT_FIELD(MqMessage,channel,data,token,flags,retryTimes);
};


}
#endif
