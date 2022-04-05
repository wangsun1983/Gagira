#ifndef __GAGRIA_HTTP_SERVLET_REQUEST_MANAGER_HPP__
#define __GAGRIA_HTTP_SERVLET_REQUEST_MANAGER_HPP__

#include <thread>

#include "Object.hpp"
#include "StrongPointer.hpp"
#include "String.hpp"
#include "HashMap.hpp"
#include "HttpRouter.hpp"
#include "HttpServer.hpp"
#include "HttpServerBuilder.hpp"
#include "HttpRouterManager.hpp"
#include "HttpListener.hpp"
#include "ThreadLocal.hpp"
#include "HttpLinker.hpp"
#include "Controller.hpp"
#include "ThreadLocal.hpp"
#include "ControllerParam.hpp"
#include "ServletRequest.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ServletRequestCache) {
public:
    _ServletRequestCache(ServletRequest,sp<_ControllerParam>);
    ServletRequest r;
    sp<_ControllerParam> p;
};

DECLARE_CLASS(ServletRequestManager) {
public:
    static sp<_ServletRequestManager> getInstance();

    void add(ServletRequestCache);
    ServletRequest getRequest();
    ControllerParam getParam();
    void remove();

private:
    _ServletRequestManager();
    static sp<_ServletRequestManager> mInstance;

    ThreadLocal<ServletRequestCache> mCaches;
};

}
#endif
