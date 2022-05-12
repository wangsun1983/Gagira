#ifndef __MY_GROUP_USER_INFO_HPP__
#define __MY_GROUP_USER_INFO_HPP__
#include <stdio.h>
#include <unistd.h>

#include "StrongPointer.hpp"
#include "Object.hpp"
#include "GroupInfo.hpp"
#include "Reflect.hpp"

using namespace obotcha;

DECLARE_CLASS(UserInfo) {
public:
    String sessionid;
    String username;
    String password;
    int userid;
    ArrayList<GroupInfo> mygroups;
    ArrayList<GroupInfo> followedGroups;
    DECLARE_REFLECT_FIELD(UserInfo,username,password,userid,mygroups,followedGroups,sessionid)
};


#endif