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

using namespace obotcha;

namespace gagira  {

DECLARE_SIMPLE_CLASS(Server) IMPLEMENTS(HttpListener){
public:
    _Server();
    _Server* setAddress(InetAddress);
    _Server* setOption(HttpOption);

    int start();
    void onHttpMessage(int event,HttpLinker client,HttpResponseWriter w,HttpPacket msg);

private:
    HttpServerBuilder mBuilder;
    HttpServer mServer;
    HttpRouterManager mRouterManager;
};

}
#endif
