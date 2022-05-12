#include "ConversationController.hpp"
#include "BaseNetData.hpp"
#include "OnlineService.hpp"
#include "ServletRequest.hpp"
#include "NoneResponseEntity.hpp"
#include "String.hpp"

_ConversationController::_ConversationController() {
    mConversationService = createConversationService();
}

HttpResponseEntity _ConversationController::onNewConversation() {
    //ByteArray data = getWebSocketRequest();
    printf("conversation controller start \n");
    ServletRequest req = getRequest();
    String s = req->getContent<String>();

    JsonReader reader = createJsonReader(s);
    JsonValue value = reader->get();

    int type = value->getInteger("type")->toValue();
    printf("conversation controller trace1,type is %d \n",type);
    
    switch(type) {
        //case ShakeHand: {
        //    String sessionId = value->getString("sessionid");
        //    st(OnlineService)::getInstance()->newWsClient(sessionId,getWebSocketClient());
        //}
        //break;

        case TextMessage: {
            String group = value->getString("groupname");
            String user = value->getString("username");
            String text = value->getString("data");
            int ret = mConversationService->addConversation(group,user,TextMessage,text,s->toByteArray());
            return createHttpResponseEntity(createString(ret));
        } 
        break;
    }

    return createHttpResponseEntity(200);
}
