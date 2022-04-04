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

using namespace obotcha;

namespace gagira  {

DECLARE_CLASS(ServletRequestManager) {
public:
    static sp<_ServletRequestManager> getInstance();

    void addRequest(ServletRequest r);
    ServletRequest getRequest();
    void removeRequest();

private:
    _ServletRequestManager();
    static std::once_flag s_flag;
    static sp<_ServletRequestManager> mInstance;
    
    ThreadLocal<ServletRequest> mRequests;
};

}
#endif
