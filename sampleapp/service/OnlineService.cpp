#include <mutex>
#include "OnlineService.hpp"
#include "GroupInfo.hpp"

sp<_OnlineService> _OnlineService::mInstance = nullptr;

sp<_OnlineService> _OnlineService::getInstance() {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        _OnlineService *p = new _OnlineService();
        p->mInstance.set_pointer(p);
    });

    return mInstance;
}

_OnlineService::_OnlineService() {
    mGroupDaoInstance = createGroupInfoDao();

    mClientsLock = createReadWriteLock();
    mClientsRdLock = mClientsLock->getReadLock();
    mClientsWrLock = mClientsLock->getWriteLock();
    mWsClients = createHashMap<String,WebSocketLinker>();
    mUsers = createHashMap<String,String>();

    mGroupMembersLock = createReadWriteLock();
    mGroupMembersRdLock = mGroupMembersLock->getReadLock();
    mGroupMembersWrLock = mGroupMembersLock->getWriteLock();
    groupMembers = createHashMap<String,ArrayList<WebSocketLinker>>();
}

void _OnlineService::newWsClient(String sessionid,WebSocketLinker client) {

    {
        AutoLock l(mClientsWrLock);
        mWsClients->put(sessionid,client);
    }
    

    //check whether the user followed the group or has his own group
    String username = nullptr;
    
    {
        AutoLock l(mClientsRdLock);
        username = mUsers->get(sessionid);
    }

    {
        AutoLock l(mGroupMembersWrLock);
        ArrayList<GroupInfo> groups = mGroupDaoInstance->getFollowedGroups(username);
        if(groups != nullptr || groups->size() > 0) {
            auto iterator = groups->getIterator();
            while(iterator->hasValue()) {
                auto group = iterator->getValue();
                ArrayList<WebSocketLinker> clients = groupMembers->get(group->group_name);
                if(clients == nullptr) {
                    clients = ArrayList<WebSocketLinker>();
                    groupMembers->put(group->group_name,clients);
                }

                clients->add(client);
                iterator->next();
            }
        }

        groups = mGroupDaoInstance->getOwnGroups(username);
        if(groups != nullptr || groups->size() > 0) {
            auto iterator = groups->getIterator();
            while(iterator->hasValue()) {
                auto group = iterator->getValue();
                ArrayList<WebSocketLinker> clients = groupMembers->get(group->group_name);
                if(clients == nullptr) {
                    clients = ArrayList<WebSocketLinker>();
                    groupMembers->put(group->group_name,clients);
                }

                clients->add(client);
                iterator->next();
            }
        }
    }
}

void _OnlineService::newUser(String sessionid,String username) {
    {
        AutoLock l(mClientsWrLock);
        mUsers->put(sessionid,username);
    }
}

void _OnlineService::foreachMember(String groupName,OnlineForeach func) {
    {
        AutoLock l(mGroupMembersRdLock);
        auto members = groupMembers->get(groupName);
        if(members != nullptr) {
            auto iterator = members->getIterator();
            while(iterator->hasValue()) {
                func(iterator->getValue());
                iterator->next();
            }
        }
    }
}