#ifndef __GAGRIA_MQ_PERSISTENT_COMPONENT_HPP__
#define __GAGRIA_MQ_PERSISTENT_COMPONENT_HPP__

#include "String.hpp"
#include "MqMessage.hpp"
#include "OStdReturnValue.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqPersistentComponent) {
public:
    virtual int newChannel(String channel) = 0;
    virtual String newMessage(String channel,ByteArray msg) = 0;
    virtual DefRet(String,ByteArray) take(String channel) = 0;
    virtual int remove(String channel,String token) = 0;
    //virtual int newDeadLetterMessage(String channel,ByteArray msg) = 0;
};

}
#endif
