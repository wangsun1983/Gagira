#ifndef __GAGRIA_QUEUE_MESSAGE_HPP__
#define __GAGRIA_QUEUE_MESSAGE_HPP__

#include "Object.hpp"
#include "Serializable.hpp"
#include "ByteArray.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(QueueMessage) IMPLEMENTS(Serializable) {
public:
    enum Type {
        Acquire = 0,
        Submit,
        ClientNoWait,
        Task,
    };

    enum ResultType {
        Ok = 0,
        IlleaglMd5sum,
    };

    uint32_t event;
    ByteArray data;

    _QueueMessage();
    _QueueMessage(int event,ByteArray data);

    DECLARE_REFLECT_FIELD(QueueMessage,event,data);
};

}

#endif
