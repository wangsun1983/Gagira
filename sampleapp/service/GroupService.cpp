#include "GroupService.hpp"

_GroupService::_GroupService() {
    groupInfoDaoInstance = GroupInfoDao::New();
    conversationDaoInstance = ConversationDao::New();
}

GroupInfo _GroupService::getGroupInfo(String groupname) {
    return groupInfoDaoInstance->getGroupInfo(groupname);
}

int _GroupService::addGroup(GroupInfo info) {
    //String group_name,String group_type,String group_owner,String group_summary
    int ret = groupInfoDaoInstance->addNewGroup(info->group_name,
                                             info->group_type,
                                             info->group_owner,
                                             info->group_summary);

    //create conversation
    ret = conversationDaoInstance->createConversation(info->group_name);
    return ret;
}

int _GroupService::followGroup(String groupname,String username) {
    return groupInfoDaoInstance->followGroup(groupname,username);
}
