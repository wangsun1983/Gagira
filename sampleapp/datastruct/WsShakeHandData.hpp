#ifndef __MY_GROUP_WEBSOCKET_SHAKE_HAND_DATA_HPP__
#define __MY_GROUP_WEBSOCKET_SHAKE_HAND_DATA_HPP__

#include "BaseNetData.hpp"
#include "Reflect.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(WsShakeHandData) IMPLEMENTS(BaseNetData) {
public:
    String sessionid;
    DECLARE_REFLECT_FIELD(WsShakeHandData,sessionid,type)
};

#endif
