#ifndef __MY_GROUP_ONLINE_SERVICE_HPP__
#define __MY_GROUP_ONLINE_SERVICE_HPP__

#include "String.hpp"
#include "ArrayList.hpp"
#include "Conversation.hpp"
#include "WebSocketLinker.hpp"
#include "ReadWriteLock.hpp"
#include "HashMap.hpp"
#include "ConcurrentQueue.hpp"
#include "GroupInfoDao.hpp"

using namespace obotcha;

using OnlineForeach = std::function<void(WebSocketLinker)>;


DECLARE_CLASS(OnlineService) {
public:
    static sp<_OnlineService> getInstance();

    void foreachMember(String groupName,OnlineForeach func);

    void newWsClient(String sessionid,WebSocketLinker client);
    void newUser(String sessionid,String username);
        
private:
    static sp<_OnlineService> mInstance;
    _OnlineService();
    
    ReadWriteLock mClientsLock;
    ReadLock mClientsRdLock;
    WriteLock mClientsWrLock;
    HashMap<String,WebSocketLinker> mWsClients;
    HashMap<String,String> mUsers;

    ReadWriteLock mGroupMembersLock;
    ReadLock mGroupMembersRdLock;
    WriteLock mGroupMembersWrLock;
    HashMap<String,ArrayList<WebSocketLinker>> groupMembers;

    GroupInfoDao mGroupDaoInstance;
};


#endif
