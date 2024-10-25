#ifndef __GAGIRA_HTTP_ASPECT_HPP__
#define __GAGIRA_HTTP_ASPECT_HPP__

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
#include "HttpResourceManager.hpp"

namespace gagira {

#define GetPacket()                                                            \
    ({                                                                         \
        st(GlobalCacheManager)::getInstance()->getPacket();                    \
    })
    
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

#define GetWebsocketRequest()                                                  \
    ({                                                                         \
        st(GlobalCacheManager)::getInstance()->getWsRequest();                 \
    })    

#define GetWebsocketClient()                                                   \
    ({                                                                         \
        st(GlobalCacheManager)::getInstance()->getWsClient();                  \
    })

template <typename T> T getClass(sp<T>) {
    Trigger(MethodNotSupportException, "cannot use this function");
}

#define InjectController(method, url, instance, function)                                       \
    {                                                                                           \
        auto func = std::bind(&decltype(getClass(instance))::function,                          \
                              instance.get_pointer());                                          \
        ControllerRouter r = ControllerRouter::New(func, instance);                             \
        HttpRouter router = HttpRouter::New(url, r);                                            \
        st(HttpRouterManager)::getInstance()->addRouter(static_cast<int>(method), router);      \
    }

#define InjectResource(path) \
    st(HttpResourceManager)::getInstance()->addSearchPath(path); 

#define InjectViewRedirect(view,path) \
    st(HttpResourceManager)::getInstance()->setViewRedirect(view,path); 

#define InjectDefaultHomePage(path) \
    st(HttpResourceManager)::getInstance()->setViewRedirect("/",path);     

#define InjectInterceptor(method, url, point, instance)                        \
    {                                                                          \
        HashMap<String, String> queries;                                       \
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

#define InjectGlobalController(method,instance,function)                       \
    {                                                                          \
        auto func = std::bind(&decltype(getClass(instance))::function,         \
                              instance.get_pointer());                         \
        ControllerRouter r = ControllerRouter::New(func, instance);            \
        st(Server)::getInstance()->addGlobalController(method, r);             \
    }

#define InjectWsController(path, instance, function)                           \
    {                                                                          \
        auto func = std::bind(&decltype(getClass(instance))::function,         \
                              instance.get_pointer());                         \
        ControllerRouter r = ControllerRouter::New(func, instance);            \
        st(WebSocketRouterManager)::getInstance()->addRouter(path, r);         \
        auto wsServer = st(Server)::getInstance()->getWebSocketServer();       \
        if(wsServer != nullptr) {                                              \
            wsServer->bind(path, st(Server)::getInstance());                   \
        } else {                                                               \
            st(Server)::getInstance()->addLazyWsRegistPath(path);              \
        }                                                                      \
    }

} // namespace gagira

#endif
