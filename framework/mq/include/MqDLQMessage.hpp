#ifndef __GAGRIA_MQ_DLQ_MESSAGE_HPP__
#define __GAGRIA_MQ_DLQ_MESSAGE_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "ByteArray.hpp"
#include "Serializable.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqDLQMessage) IMPLEMENTS(Serializable) {
public:
    enum Code {
        MessageToken = 0,
        NoClient,
        ClientDisconnect,
        MessageTimeOut,
    };

    _MqDLQMessage();

    _MqDLQMessage *setCode(int code);
    _MqDLQMessage *setData(ByteArray data);
    _MqDLQMessage *setToken(String token);
    _MqDLQMessage *setSrcAddress(String addr);
    _MqDLQMessage *setDestAddress(String addr);
    _MqDLQMessage *setPointTime(long time);

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
    DECLARE_REFLECT_FIELD(MqDLQMessage,mCode,
                          mMessageData,mSrcAddress,
                          mDestAddress,mToken,
                          mPointTime);
};

}

#endif
