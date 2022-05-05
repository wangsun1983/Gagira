#ifndef __GAGRIA_SERVER_CONFIG_HPP__
#define __GAGRIA_SERVER_CONFIG_HPP__

#include "StrongPointer.hpp"
#include "Object.hpp"
#include "ArrayList.hpp"
#include "SqlConfig.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ServerConfig) {
public:
    String ip;
    int port;
    
    DECLARE_REFLECT_FIELD(ServerConfig,ip,port);
};

} // namespace gagira

#endif