#include "ClientWsController.hpp"
#include "BaseNetData.hpp"
#include "OnlineService.hpp"

_ClientWsController::_ClientWsController() {
    //mUserService = createUserService();
}

WsResponseEntity _ClientWsController::onMessage() {
    ByteArray data = getWebSocketRequest();
    String s = data->toString();

    JsonReader reader = createJsonReader(s);
    JsonValue value = reader->get();

    int type = value->getInteger("type")->toValue();

    switch(type) {
        case ShakeHand: {
            String sessionId = value->getString("sessionid");
            st(OnlineService)::getInstance()->newWsClient(sessionId,getWebSocketClient());
        }
        break;
    }

    return nullptr;
}