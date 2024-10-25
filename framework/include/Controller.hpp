#ifndef __GAGRIA_CONTROLLER_HPP__
#define __GAGRIA_CONTROLLER_HPP__

#include <functional>

#include "Object.hpp"
#include "StrongPointer.hpp"
#include "String.hpp"
#include "HashMap.hpp"
#include "HttpRouter.hpp"
#include "HttpLinker.hpp"
#include "ServletRequest.hpp"
#include "MethodNotSupportException.hpp"
#include "WebSocketLinker.hpp"

using namespace obotcha;

namespace gagira {

using ControllerFunction = std::function<ResponseEntity()>;

DECLARE_CLASS(Controller) {
public:
    ServletRequest getRequest();
    ByteArray getWebSocketRequest();
    WebSocketLinker getWebSocketClient();
};

DECLARE_CLASS(ControllerRouter) IMPLEMENTS(RouterListener) {

public:
    _ControllerRouter(ControllerFunction c,Controller ctr);
    ResponseEntity onInvoke();

private:
    ControllerFunction func;
    Controller controller;
};

}

#endif
