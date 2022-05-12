#ifndef __MY_GROUP_LOGIN_DATA_HPP__
#define __MY_GROUP_LOGIN_DATA_HPP__
#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "Inet4Address.hpp"
#include "ServletRequest.hpp"
#include "HttpResourceManager.hpp"
#include "Reflect.hpp"
#include "Utils.hpp"
#include "Interceptor.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(LoginData) {
public:
    String username;
    String password;
    DECLARE_REFLECT_FIELD(LoginData,username,password)
};


#endif