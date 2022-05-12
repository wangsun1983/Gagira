#ifndef __MY_GROUP_CONVERSATION_SERVICE_HPP__
#define __MY_GROUP_CONVERSATION_SERVICE_HPP__

#include "GroupInfo.hpp"
#include "String.hpp"
#include "MySqlDao.hpp"
#include "ArrayList.hpp"
#include "Conversation.hpp"
#include "ConversationDao.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(ConversationService) {
public:
    _ConversationService();
    ArrayList<Conversation> getConversation(String groupname);
    ArrayList<Conversation> getConversationAfter(String groupname,long time);
    int addConversation(String groupname,String username,int type,String data,ByteArray rawData);

private:
    ConversationDao mConversationDao;
};

#endif
