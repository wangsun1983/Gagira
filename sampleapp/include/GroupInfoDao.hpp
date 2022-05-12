#ifndef __MY_GROUP_GROUP_INFO_DAO_HPP__
#define __MY_GROUP_GROUP_INFO_DAO_HPP__

#include "GroupInfo.hpp"
#include "String.hpp"
#include "MySqlDao.hpp"
#include "ArrayList.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(GroupInfoDao) IMPLEMENTS(MySqlDao) {
public:
    ArrayList<GroupInfo> getOwnGroups(String username);
    ArrayList<GroupInfo> getFollowedGroups(String username);
    GroupInfo getGroupInfo(String groupname);

    int addNewGroup(String group_name,String group_type,String group_owner,String group_summary);
    int followGroup(String group_name,String username);
};

#endif
