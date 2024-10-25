#include "Controller.hpp"
#include "HttpRouter.hpp"
#include "GlobalCacheManager.hpp"

namespace gagira  {

//------------------- Controller -----------------
ServletRequest _Controller::getRequest() {
    return st(GlobalCacheManager)::getInstance()->getRequest();
}

ByteArray _Controller::getWebSocketRequest() {
    return st(GlobalCacheManager)::getInstance()->getWsRequest();
}

WebSocketLinker _Controller::getWebSocketClient() {
    return st(GlobalCacheManager)::getInstance()->getWsClient();
}

//------------------- Controller Router -----------------
_ControllerRouter::_ControllerRouter(ControllerFunction c,Controller ctr) {
    func = c;
    controller = ctr;
}

ResponseEntity _ControllerRouter::onInvoke() {
    return func();
}

}
