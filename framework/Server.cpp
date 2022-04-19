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

HashMap<Integer,ArrayList<Interceptor>> _Server::interceptors 
            = createHashMap<Integer,ArrayList<Interceptor>>();

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
        return mServer->start();
    }

    return -1;
}

void _Server::close() {
    mServer->close();
}

void _Server::waitForExit(long interval) {
    mServer->join(interval);
}

void _Server::addinterceptors(int method,Interceptor c) {
    ArrayList<Interceptor> list = interceptors->get(createInteger(method));
    if(list == nullptr) {
        list = createArrayList<Interceptor>();
        interceptors->put(createInteger(method),list);
    }
    list->add(c);
    interceptors->put(createInteger(method),list);
}

void _Server::onHttpMessage(int event,HttpLinker client,HttpResponseWriter w,HttpPacket msg) {
    
    if(event == st(NetEvent)::Message) {
        HashMap<String,String> map = createHashMap<String,String>();
        ServletRequest req = createServletRequest(msg,client);
        //printf("client is %s \n",client->getInetAddress()->toChars());
        ServletRequestCache cache = createServletRequestCache(req,map);
        st(ServletRequestManager)::getInstance()->add(cache);

        int method = msg->getHeader()->getMethod();
        HttpResponse response = createHttpResponse();
        //add interceptor
        ArrayList<Interceptor> list = interceptors->get(createInteger(method));
        if(list != nullptr) {
            auto iterator = list->getIterator();
            while(iterator->hasValue()) {
                auto c = iterator->getValue();
                if(!c->onIntercept()) {
                    response->getHeader()->setResponseStatus(st(HttpStatus)::BadRequest);
                    w->write(response);
                    return;
                }
                iterator->next();
            }
        }
        //add interceptor

        String url = msg->getHeader()->getUrl()->getRawUrl();
        File file = mResourceManager->findResource(url);
        printf("url is %s \n",url->toChars());
        if(file != nullptr) {
            response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
            //response->setFile(file);
            HttpChunk chunk = createHttpChunk(file);
            response->getEntity()->setChunk(chunk);
            w->write(response);
            printf("url %s response \n",url->toChars());
            return;
        }
        HttpRouter router = mRouterManager->getRouter(method,url,map);
        if(router != nullptr) {
            //TODO?
            printf("http router is not nullptr \n");
            HttpEntity entity = createHttpEntity();
            

            //st(ServletRequestManager)::getInstance()->addRequest(req);
            HttpResponseEntity obj = router->invoke();
            
            
            if(obj != nullptr) {
                entity->setContent(obj->getContent()->get()->toByteArray());
                response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
                response->setEntity(entity);
                w->write(response);
                return;
            }
            //st(ServletRequestManager)::getInstance()->removeRequest();
        }

        response->getHeader()->setResponseStatus(st(HttpStatus)::BadRequest);
        w->write(response);
    }
}

}
