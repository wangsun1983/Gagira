#ifndef __GAGRIA_SERVER_HPP__
#define __GAGRIA_SERVER_HPP__

#include <functional>

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
#include "HttpResourceManager.hpp"

using namespace obotcha;

namespace gagira  {

DECLARE_CLASS(Server) IMPLEMENTS(HttpListener){
public:
    friend class _Controller;

    _Server();
    _Server* setAddress(InetAddress);
    _Server* setOption(HttpOption);

    int start();

    void close();

    void waitForExit(long interval = 0);

    void onHttpMessage(int event,HttpLinker client,HttpResponseWriter w,HttpPacket msg);
    
private:
    HttpServerBuilder mBuilder;
    HttpServer mServer;
    HttpRouterManager mRouterManager;
    HttpResourceManager mResourceManager;
};

}
#endif
