#include "ConversationService.hpp"
#include "System.hpp"
#include "OnlineService.hpp"
#include "WebSocketLinker.hpp"

_ConversationService::_ConversationService() {
    mConversationDao = createConversationDao();
}

ArrayList<Conversation> _ConversationService::getConversation(String groupname) {
    return mConversationDao->getConversation(groupname);
}

ArrayList<Conversation> _ConversationService::getConversationAfter(String groupname,long time) {
    return mConversationDao->getConversationAfter(groupname,time);
}

int _ConversationService::addConversation(String groupname,String username,int type,String data,ByteArray rawData) {
    //save conversation to data
    printf("ConversatonService,addConversation start \n");
    int ret = mConversationDao->addConversation(groupname,
                                                st(System)::currentTimeMillis(),
                                                username,
                                                type,
                                                data);

    //
    st(OnlineService)::getInstance()->foreachMember(groupname,[&rawData](WebSocketLinker c){
        c->sendBinaryMessage(rawData);
    });

    return ret;
}
