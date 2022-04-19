#ifndef __GAGIRA_HTTP_UTILS_HPP__
#define __GAGIRA_HTTP_UTILS_HPP__

#include <functional>

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "HashMap.hpp"
#include "HttpResponseEntity.hpp"
#include "HttpRouter.hpp"
#include "HttpRouterManager.hpp"
#include "Interceptor.hpp"
#include "ServletRequestManager.hpp"
#include "String.hpp"
#include "TextContent.hpp"

namespace gagira {

#define GetIntParam(key)                                                       \
    ({                                                                         \
        auto param = st(ServletRequestManager)::getInstance()->getParam();     \
        param->get<Integer>(#key);                                             \
    })

#define GetBoolParam(key)                                                      \
    ({                                                                         \
        auto param = st(ServletRequestManager)::getInstance()->getParam();     \
        param->get<Boolean>(#key);                                             \
    })

#define GetStringParam(key)                                                    \
    ({                                                                         \
        auto param = st(ServletRequestManager)::getInstance()->getParam();     \
        param->get<String>(#key);                                              \
    })

template <typename T> T getClass(sp<T>) {
    Trigger(MethodNotSupportException, "cannot use this function");
}

#define InjectController(method, url, instance, function)                      \
    {                                                                          \
        auto func = std::bind(&decltype(getClass(instance))::function,         \
                              instance.get_pointer());                         \
        ControllerRouter r = createControllerRouter(func, instance);           \
        HttpRouter router = createHttpRouter(url, r);                          \
        st(HttpRouterManager)::getInstance()->addRouter(method, router);       \
    }

#define InjectInterceptor(method, url, point, instance)                        \
    {                                                                          \
        HashMap<String, String>                                                \
            queries = createHashMap<String, String>();                         \
        HttpRouter router = st(HttpRouterManager)::getInstance()->getRouter(   \
            method, url, queries);                                          \
        if (point == st(Interceptor)::BeforeExec) {                            \
            router->addBeforeExecInterceptor(instance);                        \
        } else if (point == st(Interceptor)::AfterExec) {                      \
            router->addAfterExecInterceptor(instance);                         \
        }                                                                      \
    }

#define InjectResInterceptor(path, instance)                                   \
    {                                                                          \
        st(HttpResourceManager)::getInstance()                                 \
            ->addResourceInterceptor(path, instance);                          \
    }

#define InjectGlobalInterceptor(method,instance) \
    {\
        st(Server)::addinterceptors(method,instance);\
    }

} // namespace gagira

#endif
