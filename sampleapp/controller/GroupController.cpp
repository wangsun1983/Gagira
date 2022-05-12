#include "GroupController.hpp"

_GroupController::_GroupController() {
    mGroupService = createGroupService();
}

HttpResponseEntity _GroupController::addNewGroup() {
    ServletRequest req = getRequest();
    GroupInfo info = req->getContent<GroupInfo>();
    int ret = mGroupService->addGroup(info);
    return createHttpResponseEntity(createString(ret));
}
    
