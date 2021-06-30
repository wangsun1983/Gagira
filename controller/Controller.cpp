#include "Controller.hpp"
#include "HttpRouter.hpp"

namespace gagira  {

_ControllerRouter::_ControllerRouter(HttpControllerFunction c,HttpController ctr) {
    func = c;
    controller = ctr;
}

Object _ControllerRouter::onInvoke(HashMap<String,String> m) {
    return func(m);
}

}
