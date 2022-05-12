#ifndef __MY_GROUP_CONVERSATION_DAO_HPP__
#define __MY_GROUP_CONVERSATION_DAO_HPP__

#include "GroupInfo.hpp"
#include "String.hpp"
#include "MySqlDao.hpp"
#include "ArrayList.hpp"
#include "Conversation.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(ConversationDao) IMPLEMENTS(MySqlDao) {
public:
    ArrayList<Conversation> getConversation(String groupname);
    ArrayList<Conversation> getConversationAfter(String groupname,long time);
    int addConversation(String groupname,long time,String username,int type,String data);
    int createConversation(String groupname);
};

#endif
