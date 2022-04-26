#ifndef __GAGRIA_MQ_UNDELIVERED_COMPONENT_HPP__
#define __GAGRIA_MQ_UNDELIVERED_COMPONENT_HPP__

#include "String.hpp"
#include "MqMessage.hpp"
#include "MqPersistentComponent.hpp"
#include "OStdReturnValue.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqUndeliveredComponent) IMPLEMENTS(MqPersistentComponent) {
public:
    _MqUndeliveredComponent();
    int newChannel(String channel);
    String newMessage(String channel,ByteArray msg);
    DefRet(String,ByteArray) take(String channel);
    int remove(String channel,String token);

private:
    Mutex mMutex;
    HashMap<String,LinkedList<ByteArray>> mDatas;
};

}
#endif
