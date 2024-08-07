#include "UserService.hpp"
#include "LoginData.hpp"
#include "LoginController.hpp"
#include "Utils.hpp"

using namespace obotcha;
using namespace gagira;

_LoginController::_LoginController() {
    mUserService = UserService::New();
}

HttpResponseEntity _LoginController::login() {
    ServletRequest req = getRequest();
    String username = GetStringParam(username);
    String password = GetStringParam(password);

    UserInfo info = mUserService->logIn(username,password);
    return HttpResponseEntity::New(info);
}

