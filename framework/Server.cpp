#include "Controller.hpp"
#include "HttpRouter.hpp"
#include "Server.hpp"
#include "Error.hpp"
#include "HttpEntity.hpp"
#include "HttpResponse.hpp"
#include "HttpServer.hpp"
#include "ServletRequestManager.hpp"
#include "HttpResourceManager.hpp"

using namespace obotcha;

namespace gagira  {

_Server::_Server() {
    mBuilder = createHttpServerBuilder();
    mServer = nullptr;
    mRouterManager = st(HttpRouterManager)::getInstance();
    mResourceManager = st(HttpResourceManager)::getInstance();
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
    mBuilder->setListener(AutoClone(this));
    mServer = mBuilder->build();
    
    if(mServer != nullptr) {
        mServer->start();
        return 0;
    }

    return -NotStart;
}

void _Server::onHttpMessage(int event,HttpLinker client,HttpResponseWriter w,HttpPacket msg) {
    if(event != 0) {
        int method = msg->getHeader()->getMethod();
        String url = msg->getHeader()->getUrl()->getRawUrl();
        File file = mResourceManager->findResource(url);
        printf("url is %s \n",url->toChars());
        if(file != nullptr) {
            HttpResponse response = createHttpResponse();
            response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
            response->setFile(file);
            w->write(response);
            printf("url %s response \n",url->toChars());
            return;
        }

        HashMap<String,String> map = createHashMap<String,String>();
        HttpRouter router = mRouterManager->getRouter(method,url,map);
        if(router != nullptr) {
            //TODO?
            HttpResponseEntity obj = router->getListener()->onInvoke(map);
            HttpEntity entity = createHttpEntity();
            ServletRequest req = createServletRequest(msg,client);
            st(ServletRequestManager)::getInstance()->addRequest(req);
            entity->setContent(createByteArray(obj->getContent()->get()));
            st(ServletRequestManager)::getInstance()->getRequest();

            HttpResponse response = createHttpResponse();
            response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
            response->setEntity(entity);
            w->write(response);
        }
    }
}

}
