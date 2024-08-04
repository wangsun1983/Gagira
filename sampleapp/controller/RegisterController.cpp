#include "RegisterController.hpp"

_RegisterController::_RegisterController() {
    mUserService = UserService::New();
}

HttpResponseEntity _RegisterController::registUser() {
    printf("registUser!!! \n");
    ServletRequest req = getRequest();
    UserInfo info = req->getContent<UserInfo>();
    int result = mUserService->addNewUser(info);

    return HttpResponseEntity::New(result);
}

