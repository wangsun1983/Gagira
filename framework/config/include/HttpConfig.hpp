#ifndef __GAGRIA_HTTP_SERVER_CONFIG_HPP__
#define __GAGRIA_HTTP_SERVER_CONFIG_HPP__

#include "StrongPointer.hpp"
#include "Object.hpp"
#include "ArrayList.hpp"
#include "SqlConfig.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HttpConfig) {
public:
    String ip;
    int port;
    
    DECLARE_REFLECT_FIELD(HttpConfig,ip,port);
};

} // namespace gagira

#endif