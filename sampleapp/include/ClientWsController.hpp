#ifndef __MY_GROUP_WS_CLIENT_CONTROLLER_HPP__
#define __MY_GROUP_WS_CLIENT_CONTROLLER_HPP__

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
#include "UserService.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(ClientWsController) IMPLEMENTS(Controller) {
public:
    _ClientWsController();

    WsResponseEntity onMessage();

private:
    UserService mUserService;
};

#endif
