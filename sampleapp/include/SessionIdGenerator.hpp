#ifndef __MY_GROUP_SESSION_ID_GENERATOR_HPP__
#define __MY_GROUP_SESSION_ID_GENERATOR_HPP__

#include "String.hpp"
#include "UserInfo.hpp"
#include "UserInfoDao.hpp"
#include "GroupInfoDao.hpp"
#include "Mutex.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(SessionIdGenerator) {
public:
    static String toString(String username,String ip);

};

#endif
