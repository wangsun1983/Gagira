#ifndef __MY_GROUP_TEST_CONTROLLER_HPP__
#define __MY_GROUP_TEST_CONTROLLER_HPP__

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
#include "GroupService.hpp"
#include "ConversationService.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(TestController) IMPLEMENTS(Controller) {
public:
    _TestController();

    HttpResponseEntity testNewGroup();

private:
    UserService mUserService;
    GroupService mGroupService;
    ConversationService mConversationService;
};

#endif
