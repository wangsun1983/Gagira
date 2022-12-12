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

    enum MessageSymbol {
        //message type
        ShakeHand = 1<<1,
        Subscribe = 1<<2,
        UnSubscribe = 1<<3,
        Publish = 1<<4,
        //Server will remove msg when accept one ack message
        //if no ack return in time,server will resend the message.
        Ack = 1<<5,
        //1.Response for unsubscribe.....
        //2.If client was removed from server,send this message.
        Detach = 1<<6,

        //send this message to make mq send send persistent 
        //message to the client to save.
        SubscribePersistence = 1<<7,
        PostBack = 1<<8,

        //regist as dlq,mq center will send message which failed 
        //to be sent to this client
        SubscribeDLQ = 1<<9,
        
        //Center send info to client for example:
        //1.waiting for persist data synchronization
        //2.too many work,do not send for a moment
        //3.DLQ message
        //and so on
        Sustain = 1<<15,

        MaxMessageType = 1<<16,

        //psersist
        OneShotFlag = 1 <<18,
        AcknowledgeFlag = 1<<19,
        StickFlag = 1<<20,
        UnStickFlag = 1<<21,
        PersistFlag = 1<<22,
        StartFalg = 1<<23,
        CompleteFlag = 1<<24,
        DelayFlag = 1<<25,//TODO
    };

    _MqMessage();

    _MqMessage(String channel,ByteArray data,uint32_t types);

    _MqMessage(String channel,String stickToken,ByteArray data,uint32_t types);

    ByteArray generatePacket();

    static MqMessage generateMessage(ByteArray);

    void setData(ByteArray data);
    void setChannel(String channel);
    void setAckToken(String);
    void setFlags(uint32_t flags);
    void setStickToken(String);
    void setRetryTimes(int);
    void setTTL(long);
    void setPublishTime(long);

    ByteArray getData();
    void clearData();

    String getChannel();
    uint32_t getType();
    bool isOneShot();
    bool isAcknowledge();
    bool isStick();
    bool isUnStick();
    bool isPersist();
    bool isStart();
    bool isComplete();
    
    String getAckToken();
    String getStickToken();
    int getRetryTimes();
    long getTTL();
    long getExpireTime();
    long getPublishTime();

private:
    ByteArray mData;
    String mChannel;
    String mAckToken;
    uint32_t mFlags;
    Socket mSocket;
    int mRetryTimes;
    String mStickToken;

    long mTTL;
    long mExpireTime;
    long mNextPublishTime;

public:
    DECLARE_REFLECT_FIELD(MqMessage,mChannel,mStickToken,mData,mAckToken,mFlags,mExpireTime,mNextPublishTime);
};


}
#endif
