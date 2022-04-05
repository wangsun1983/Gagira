#include "Controller.hpp"
#include "HttpRouter.hpp"
#include "ServletRequestManager.hpp"

namespace gagira  {

//------------------- Controller Router -----------------
ServletRequest _Controller::getRequest() {
    return st(ServletRequestManager)::getInstance()->getRequest();
}

//------------------- Controller Router -----------------
_ControllerRouter::_ControllerRouter(ControllerFunction c,Controller ctr) {
    func = c;
    controller = ctr;
}

HttpResponseEntity _ControllerRouter::onInvoke() {
    return func();
}

}
