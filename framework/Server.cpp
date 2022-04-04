#include "Controller.hpp"
#include "HttpRouter.hpp"
#include "Server.hpp"
#include "Error.hpp"
#include "HttpEntity.hpp"
#include "HttpResponse.hpp"
#include "HttpServer.hpp"
#include "ServletRequestManager.hpp"
#include "HttpResourceManager.hpp"
#include "NetEvent.hpp"

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

    return -1;
}

void _Server::onHttpMessage(int event,HttpLinker client,HttpResponseWriter w,HttpPacket msg) {
    
    if(event == st(NetEvent)::Message) {
        int method = msg->getHeader()->getMethod();
        String url = msg->getHeader()->getUrl()->getRawUrl();
        File file = mResourceManager->findResource(url);
        printf("url is %s \n",url->toChars());
        if(file != nullptr) {
            HttpResponse response = createHttpResponse();
            response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
            //response->setFile(file);
            HttpChunk chunk = createHttpChunk(file);
            response->getEntity()->setChunk(chunk);
            w->write(response);
            printf("url %s response \n",url->toChars());
            return;
        }

        HashMap<String,String> map = createHashMap<String,String>();
        HttpRouter router = mRouterManager->getRouter(method,url,map);
        if(router != nullptr) {
            //TODO?
            printf("http router is not nullptr \n");
            HttpEntity entity = createHttpEntity();
            ServletRequest req = createServletRequest(msg,client);
            //printf("client is %s \n",client->getInetAddress()->toChars());
            st(ServletRequestManager)::getInstance()->addRequest(req);
            HttpResponseEntity obj = router->getListener()->onInvoke(map);
            entity->setContent(obj->getContent()->get()->toByteArray());
            
            HttpResponse response = createHttpResponse();
            response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
            response->setEntity(entity);
            w->write(response);
            st(ServletRequestManager)::getInstance()->removeRequest();
        }
    }
}

}
