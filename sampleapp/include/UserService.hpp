#ifndef __MY_GROUP_USER_SERVICE_HPP__
#define __MY_GROUP_USER_SERVICE_HPP__

#include "String.hpp"
#include "UserInfo.hpp"
#include "UserInfoDao.hpp"
#include "GroupInfoDao.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(UserService) {
public:
    _UserService();
    UserInfo getUserInfo(String username,String password);

    UserInfo logIn(String username,String password);

    int addNewUser(UserInfo);
    
    
private:
    UserInfoDao userInfoDaoInstance;
    GroupInfoDao groupInfoDaoInstance;
};

#endif
