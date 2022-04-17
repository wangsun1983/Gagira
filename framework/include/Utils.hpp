#ifndef __GAGIRA_HTTP_UTILS_HPP__
#define __GAGIRA_HTTP_UTILS_HPP__

#include <functional>

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "HashMap.hpp"
#include "String.hpp"
#include "TextContent.hpp"
#include "HttpResponseEntity.hpp"
#include "ServletRequestManager.hpp"
#include "HttpRouterManager.hpp"
#include "HttpRouter.hpp"

namespace gagira {

#define GetIntParam(key) \
    ({\
        auto param = st(ServletRequestManager)::getInstance()->getParam(); \
        param->get<Integer>(#key);\
    })

#define GetBoolParam(key) \
    ({\
        auto param = st(ServletRequestManager)::getInstance()->getParam(); \
        param->get<Boolean>(#key);\
    })

#define GetStringParam(key) \
    ({\
        auto param = st(ServletRequestManager)::getInstance()->getParam(); \
        param->get<String>(#key);\
    })

template <typename T>
T getClass(sp<T>) {
    Trigger(MethodNotSupportException,"cannot use this function");    
}


#define Inject(method,url,instance,function) \
    {\
        auto func = std::bind(&decltype(getClass(instance))::function,instance.get_pointer());\
        ControllerRouter r = createControllerRouter(func,instance); \
        HttpRouter router = createHttpRouter(url,r);\
        st(HttpRouterManager)::getInstance()->addRouter(method,router);\
    }\


} // namespace gagira

#endif
