#include "UserService.hpp"
#include "GroupInfoDao.hpp"
#include "SessionIdGenerator.hpp"
#include "ServletRequest.hpp"
#include "GlobalCacheManager.hpp"
#include "OnlineService.hpp"

_UserService::_UserService() {
    userInfoDaoInstance = UserInfoDao::New();
    groupInfoDaoInstance = GroupInfoDao::New();
}

UserInfo _UserService::getUserInfo(String username,String password) {
    UserInfo userInfo = userInfoDaoInstance->getUserInfo(username,password);
    userInfo->mygroups = groupInfoDaoInstance->getOwnGroups(username);
    userInfo->followedGroups = groupInfoDaoInstance->getFollowedGroups(username);
    return userInfo;
}

UserInfo _UserService::logIn(String username,String password) {
    UserInfo userInfo = userInfoDaoInstance->getUserInfo(username,password);
    if(userInfo != nullptr) {
        ServletRequest req = st(GlobalCacheManager)::getInstance()->getRequest();
        String address = req->getInetAddress();
        String sessionId = st(SessionIdGenerator)::toString(username,address);
        userInfo->sessionid = sessionId;

        userInfo->mygroups = groupInfoDaoInstance->getOwnGroups(username);
        userInfo->followedGroups = groupInfoDaoInstance->getFollowedGroups(username);

        //we should add new user to online user
        st(OnlineService)::getInstance()->newUser(sessionId,username);
        
        return userInfo;
    }

    return nullptr;
}

int _UserService::addNewUser(UserInfo user) {
    return userInfoDaoInstance->addNewUser(user->username,user->password); 
}


