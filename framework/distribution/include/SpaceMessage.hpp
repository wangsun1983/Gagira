#ifndef __GAGRIA_SPACE_MESSAGE_HPP__
#define __GAGRIA_SPACE_MESSAGE_HPP__

#include "Object.hpp"
#include "Serializable.hpp"
#include "ByteArray.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(SpaceMessage) IMPLEMENTS(Serializable) {
public:
    enum Type {
        ConnectMessage = 0,
        Monitor,
        UnMonitor,
        Update,
        Get,
        Delete
    };

    uint32_t event;
    ByteArray data;
    String token;
    String md5sum;

    // static sp<_SpaceMessage> generateMessage(ByteArray);
    // ByteArray generatePacket();

    DECLARE_REFLECT_FIELD(SpaceMessage,event,data,token,md5sum);
};

}

#endif
