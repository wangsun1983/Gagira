#ifndef __GAGRIA_BROADCAST_DLQ_MESSAGE_HPP__
#define __GAGRIA_BROADCAST_DLQ_MESSAGE_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "ByteArray.hpp"
#include "Serializable.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(BroadcastDLQMessage) IMPLEMENTS(Serializable) {
public:
    enum Code {
        MessageToken = 0,
        NoClient,
        ClientDisconnect,
        MessageTimeOut,
    };

    _BroadcastDLQMessage();

    _BroadcastDLQMessage *setCode(int code);
    _BroadcastDLQMessage *setData(ByteArray data);
    _BroadcastDLQMessage *setToken(String token);
    _BroadcastDLQMessage *setSrcAddress(String addr);
    _BroadcastDLQMessage *setDestAddress(String addr);
    _BroadcastDLQMessage *setPointTime(long time);

    int getCode();
    ByteArray getData();
    String getToken();
    String getSrcAddress();
    String getDestAddress();
    long getPointTime();
    
private:
    int mCode;
    ByteArray mMessageData;
    String mSrcAddress;
    String mDestAddress;
    String mToken;
    long mPointTime;

public:
    DECLARE_REFLECT_FIELD(BroadcastDLQMessage,mCode,
                          mMessageData,mSrcAddress,
                          mDestAddress,mToken,
                          mPointTime);
};

}

#endif
