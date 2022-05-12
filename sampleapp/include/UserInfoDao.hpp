#ifndef __MY_GROUP_USER_INFO_DAO_HPP__
#define __MY_GROUP_USER_INFO_DAO_HPP__

#include "UserInfo.hpp"
#include "String.hpp"
#include "MySqlDao.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(UserInfoDao) IMPLEMENTS(MySqlDao) {
public:
    UserInfo getUserInfo(String username,String password = nullptr);
    int addNewUser(String username,String password);
};

#endif
