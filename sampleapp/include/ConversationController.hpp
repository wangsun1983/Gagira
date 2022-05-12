#ifndef __MY_GROUP_CONVERSATION_CONTROLLER_HPP__
#define __MY_GROUP_CONVERSATION_CONTROLLER_HPP__

#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "Inet4Address.hpp"
#include "ServletRequest.hpp"
#include "HttpResourceManager.hpp"
#include "Reflect.hpp"
#include "Utils.hpp"
#include "Interceptor.hpp"
#include "UserService.hpp"
#include "GroupService.hpp"
#include "ConversationService.hpp"
#include "JsonReader.hpp"
#include "WebSocketRouterManager.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(ConversationController) IMPLEMENTS(Controller) {
public:
    _ConversationController();
    HttpResponseEntity onNewConversation();

private:
    ConversationService mConversationService;
};

#endif
