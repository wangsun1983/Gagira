#include "GroupController.hpp"

_GroupController::_GroupController() {
    mGroupService = GroupService::New();
}

HttpResponseEntity _GroupController::addNewGroup() {
    ServletRequest req = getRequest();
    GroupInfo info = req->getContent<GroupInfo>();
    int ret = mGroupService->addGroup(info);
    return HttpResponseEntity::New(String::New(ret));
}
    
