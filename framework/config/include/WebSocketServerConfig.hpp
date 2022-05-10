#ifndef __GAGRIA_WEBSOCKET_SERVER_CONFIG_HPP__
#define __GAGRIA_WEBSOCKET_SERVER_CONFIG_HPP__

#include "StrongPointer.hpp"
#include "Object.hpp"
#include "ArrayList.hpp"
#include "SqlConfig.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(WebSocketServerConfig) {
public:
    String ip;
    int port;
    
    DECLARE_REFLECT_FIELD(WebSocketServerConfig,ip,port);
};

} // namespace gagira

#endif