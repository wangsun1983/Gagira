#ifndef __GAGRIA_WEBSOCKET_SERVER_CONFIG_HPP__
#define __GAGRIA_WEBSOCKET_SERVER_CONFIG_HPP__

#include "Object.hpp"
#include "String.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(WsConfig) {
public:
    String ip;
    int port;
    
    DECLARE_REFLECT_FIELD(WsConfig,ip,port);
};

} // namespace gagira

#endif