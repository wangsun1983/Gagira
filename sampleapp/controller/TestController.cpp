#include "TestController.hpp"

using namespace obotcha;
using namespace gagira;

_TestController::_TestController() {
    mUserService = UserService::New();
    mGroupService = GroupService::New();
    mConversationService = ConversationService::New();
}

HttpResponseEntity _TestController::testNewGroup() {
    GroupInfo info = GroupInfo::New();
    printf("testNetGroup Controller trace1 \n");
    info->group_name = String::New("test_name");
    info->group_type = String::New("test_type");
    info->group_summary = String::New("test_summary");
    info->group_owner = String::New("test");

    int ret = mGroupService->addGroup(info);
    return HttpResponseEntity::New(ret);
}

