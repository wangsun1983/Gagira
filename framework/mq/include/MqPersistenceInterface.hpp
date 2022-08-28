#ifndef __GAGRIA_MQ_PERSISTENCE_INTERFACE_HPP__
#define __GAGRIA_MQ_PERSISTENCE_INTERFACE_HPP__

#include "String.hpp"
#include "MqMessage.hpp"
#include "OStdReturnValue.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqPersistenceInterface) {
public:
    virtual int init() = 0;
    virtual String onNewMessage(String channel,ByteArray msg,int flag) = 0;
    virtual String onFail(String channel,ByteArray msg,int flag) = 0;
    virtual DefRet(String,ByteArray,int) onTakeMessage(String channel) = 0;
    virtual DefRet(String,ByteArray,int) onTakeFailMessage(String channel) = 0;
    virtual int onRemove(String channel,String token) = 0;
};

}
#endif
