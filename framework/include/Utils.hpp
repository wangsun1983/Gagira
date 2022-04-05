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
    st(ServletRequestManager)::getInstance()->getParam()->get<int>(#key)

#define GetBoolParam(key) \
    st(ServletRequestManager)::getInstance()->getParam()->get<bool>(#key)

#define GetStringParam(key) \
    st(ServletRequestManager)::getInstance()->getParam()->get<String>(#key)


#define Inject(method,url,instance,function) \
    {\
    auto func = std::bind(&decltype(getClass(instance))::function,instance.get_pointer());\
    ControllerRouter r = createControllerRouter(func,instance); \
    HttpRouter router = createHttpRouter(url,r);\
    st(HttpRouterManager)::getInstance()->addRouter(method,router);\
    }\

} // namespace gagira

#endif
