#ifndef __GAGIRA_HTTP_UTILS_HPP__
#define __GAGIRA_HTTP_UTILS_HPP__

#include <functional>

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "GlobalCacheManager.hpp"
#include "HashMap.hpp"
#include "HttpResponseEntity.hpp"
#include "HttpRouter.hpp"
#include "HttpRouterManager.hpp"
#include "Interceptor.hpp"
#include "String.hpp"
#include "TextContent.hpp"
#include "WebSocketRouterManager.hpp"

namespace gagira {

#define GetIntParam(key)                                                       \
    ({                                                                         \
        auto param = st(GlobalCacheManager)::getInstance()->getParam();        \
        param->get<Integer>(#key);                                             \
    })

#define GetBoolParam(key)                                                      \
    ({                                                                         \
        auto param = st(GlobalCacheManager)::getInstance()->getParam();        \
        param->get<Boolean>(#key);                                             \
    })

#define GetStringParam(key)                                                    \
    ({                                                                         \
        auto param = st(GlobalCacheManager)::getInstance()->getParam();        \
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
        HashMap<String, String> queries;                                       \
        HttpRouter router;                                                     \
        FetchRet(router, queries) =                                            \
            st(HttpRouterManager)::getInstance()->getRouter(method, url);      \
        if (point == st(Interceptor)::BeforeExec) {                            \
            router->addBeforeExecInterceptor(instance);                        \
        } else if (point == st(Interceptor)::AfterExec) {                      \
            router->addAfterExecInterceptor(instance);                         \
        }                                                                      \
    }

#define InjectResInterceptor(path, instance)                                   \
    {                                                                          \
        st(HttpResourceManager)::getInstance()->addResourceInterceptor(        \
            path, instance);                                                   \
    }

#define InjectGlobalInterceptor(method, instance)                              \
    { st(Server)::addinterceptors(method, instance); }

#define InjectWebSocketController(path, instance, function)                    \
    {                                                                          \
        auto func = std::bind(&decltype(getClass(instance))::function,         \
                              instance.get_pointer());                         \
        ControllerRouter r = createControllerRouter(func, instance);           \
        st(WebSocketRouterManager)::getInstance()->addRouter(path, r);         \
        st(Server)::getInstance()->getWebSocketServer()->bind(                 \
            path, st(Server)::getInstance());                                  \
    }

} // namespace gagira

#endif
