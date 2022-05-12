#include "TestController.hpp"

using namespace obotcha;
using namespace gagira;

_TestController::_TestController() {
    mUserService = createUserService();
    mGroupService = createGroupService();
    mConversationService = createConversationService();
}

HttpResponseEntity _TestController::testNewGroup() {
    GroupInfo info = createGroupInfo();
    printf("testNetGroup Controller trace1 \n");
    info->group_name = createString("test_name");
    info->group_type = createString("test_type");
    info->group_summary = createString("test_summary");
    info->group_owner = createString("test");

    int ret = mGroupService->addGroup(info);
    return createHttpResponseEntity(ret);
}

