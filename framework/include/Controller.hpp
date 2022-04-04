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

using namespace obotcha;

namespace gagira {

using ControllerFunction = std::function<HttpResponseEntity(HashMap<String,String>)>;

DECLARE_CLASS(Controller) {
public:
    ServletRequest getRequest();
};

DECLARE_CLASS(ControllerRouter) IMPLEMENTS(RouterListener) {

public:
    _ControllerRouter(ControllerFunction c,Controller ctr);
    HttpResponseEntity onInvoke(HashMap<String,String> m);

private:
    ControllerFunction func;
    Controller controller;
};

template <typename T>
T getClass(sp<T>) {
    Trigger(MethodNotSupportException,"cannot use this function");    
}

#define Inject(method,url,instance,function) \
    {\
    auto func = std::bind(&decltype(getClass(instance))::function,instance.get_pointer(),std::placeholders::_1);\
    ControllerRouter r = createControllerRouter(func,instance); \
    HttpRouter router = createHttpRouter(url,r);\
    st(HttpRouterManager)::getInstance()->addRouter(method,router);\
    }\

}

#endif
