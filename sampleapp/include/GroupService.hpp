#ifndef __MY_GROUP_GROUP_SERVICE_HPP__
#define __MY_GROUP_GROUP_SERVICE_HPP__

#include "String.hpp"
#include "UserInfo.hpp"
#include "UserInfoDao.hpp"
#include "GroupInfoDao.hpp"
#include "ConversationDao.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(GroupService) {
public:
    _GroupService();
    GroupInfo getGroupInfo(String groupname);
    int addGroup(GroupInfo);
    int followGroup(String groupname,String username);
    
private:
    GroupInfoDao groupInfoDaoInstance;
    ConversationDao conversationDaoInstance;
};

#endif
