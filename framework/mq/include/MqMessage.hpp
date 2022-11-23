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
        //Server will remove msg when accept one ack message
        //if no ack return in time,server will resend the message.
        MessageAck = 1<<6, 

        MaxMessageType = 1<<7,

        //1.Response for unsubscribe.....
        //2.If client was removed from server,send this message.
        Detach = 1<<8,

        //psersist
        Persistent = 1<<10,
        Acknowledge = 1<<11,

        Stick = 1<<12,
        UnStick = 1<<13,
    };

    _MqMessage();

    _MqMessage(String channel,ByteArray data,uint32_t types);

    _MqMessage(String channel,String stickTag,ByteArray data,uint32_t types);

    ByteArray generatePacket();

    static MqMessage generateMessage(ByteArray);

    ByteArray getData();
    void clearData();

    String getChannel();

    bool isPersist();

    bool isAcknowledge();

    bool isShakeHand();

    bool isSubscribe();

    bool isUnSubscribe();

    bool isPublish();

    bool isPublishOneShot();

    bool isDetach();

    bool isAck();

    bool isStick();

    bool isUnStick();

    String getToken();

    void setToken(String);

    void setFlags(uint32_t);

    uint32_t getFlags();

    int getRetryTimes();

    void setRetryTimes(int);

    String getStickTag();

private:
    ByteArray data;
    String channel;
    String token;
    uint32_t flags;

    Socket mSocket;

    int retryTimes;

    String stickTag;

public:
    DECLARE_REFLECT_FIELD(MqMessage,channel,stickTag,data,token,flags);
};


}
#endif
