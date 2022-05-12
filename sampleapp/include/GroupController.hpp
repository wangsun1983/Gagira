#ifndef __MY_GROUP_GROUP_CONTROLLER_HPP__
#define __MY_GROUP_GROUP_CONTROLLER_HPP__

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
#include "GroupService.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(GroupController) IMPLEMENTS(Controller) {
public:
    _GroupController();

    HttpResponseEntity addNewGroup();

private:
    GroupService mGroupService;
};

#endif
