#include "Controller.hpp"
#include "HttpRouter.hpp"
#include "Server.hpp"
#include "Error.hpp"
#include "HttpEntity.hpp"
#include "HttpResponse.hpp"
#include "HttpServer.hpp"

using namespace obotcha;

namespace gagira  {

_Server::_Server() {
    mBuilder = createHttpServerBuilder();
    mServer = nullptr;
    mRouterManager = st(HttpRouterManager)::getInstance();
}

_Server* _Server::setAddress(InetAddress addr) {
    mBuilder->setAddress(addr);
    return this;
}

_Server* _Server::setOption(HttpOption option) {
    mBuilder->setOption(option);
    return this;
}

int _Server::start() {
    mServer = mBuilder->build();
    if(mServer != nullptr) {
        mServer->start();
        return 0;
    }

    return -NotStart;
}

void _Server::onHttpMessage(int event,HttpLinker client,HttpResponseWriter w,HttpPacket msg) {
    int method = msg->getHeader()->getMethod();
    HashMap<String,String> map = createHashMap<String,String>();
    HttpRouter router = mRouterManager->getRouter(method,msg->getHeader()->getUrl()->getRawUrl(),map);
    if(router != nullptr) {
        //TODO
        HttpResponseEntity obj = router->getListener()->onInvoke(map);
        HttpEntity entity = createHttpEntity();
        entity->setContent(createByteArray(obj->getContent()->get()));
        HttpResponse response = createHttpResponse();
        response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
        response->setEntity(entity);
        w->write(response);
    }
}

}
