#ifndef __MY_GROUP_REGISTER_CONTROLLER_HPP__
#define __MY_GROUP_REGISTER_CONTROLLER_HPP__

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

DECLARE_CLASS(RegisterController) IMPLEMENTS(Controller) {
public:
    _RegisterController();

    HttpResponseEntity registUser();

private:
    UserService mUserService;
};

#endif
