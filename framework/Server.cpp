#include "Controller.hpp"
#include "HttpRouter.hpp"
#include "Server.hpp"
#include "Error.hpp"
#include "HttpEntity.hpp"
#include "HttpResponse.hpp"
#include "HttpServer.hpp"
#include "GlobalCacheManager.hpp"
#include "HttpResourceManager.hpp"
#include "WebSocketRouterManager.hpp"
#include "NetEvent.hpp"
#include "Configs.hpp"
#include "Inet4Address.hpp"
#include "Inet6Address.hpp"

using namespace obotcha;

namespace gagira  {

sp<_Server> _Server::mInstance = nullptr;

HashMap<Integer,ArrayList<Interceptor>> _Server::interceptors 
            = createHashMap<Integer,ArrayList<Interceptor>>();

_Server::_Server() {
    mBuilder = createHttpServerBuilder();
    mServer = nullptr;
    mRouterManager = st(HttpRouterManager)::getInstance();
    mResourceManager = st(HttpResourceManager)::getInstance();

    mInstance = AutoClone(this);
}

sp<_Server> _Server::getInstance() {
    return mInstance;
}

WebSocketServer _Server::getWebSocketServer() {
    return this->mWsServer;
}

_Server* _Server::setAddress(InetAddress addr) {
    mBuilder->setAddress(addr);
    return this;
}

_Server* _Server::setOption(HttpOption option) {
    mBuilder->setOption(option);
    return this;
}

_Server* _Server::setConfigFile(String path) {
    st(Configs)::getInstance()->load(createFile(path));

    //init http server
    String ip = st(Configs)::getInstance()->getHttpServerAddress();
    int port = st(Configs)::getInstance()->getHttpServerPort();

    InetAddress address;
    if(ip->contains(":")) {
        address = createInet6Address(ip,port);
    } else {
        address = createInet4Address(ip,port);
    }
    mBuilder->setAddress(address);
    mBuilder->setListener(AutoClone(this));
    mServer = mBuilder->build();

    //init websocket server
    String ws_ip = st(Configs)::getInstance()->getWebSocketServerAddress();
    int ws_port = st(Configs)::getInstance()->getWebSocketServerPort();

    if(ws_ip != nullptr && ws_port != -1) {
        this->mWebSocketBuilder = createWebSocketServerBuilder();
        InetAddress address;
        if(ws_ip->contains(":")) {
            address = createInet6Address(ws_ip,ws_port);
        } else {
            address = createInet4Address(ws_ip,ws_port);
        }
        mWebSocketBuilder->setInetAddr(address);
        mWsServer = mWebSocketBuilder->build();
    }   
    return this;
}

int _Server::start() {
    if(mServer != nullptr) {
        mServer->start();
    }

    if(mWsServer != nullptr) {
        mWsServer->start();
    }

    return 0;
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

//websocket
int _Server::onData(WebSocketFrame frame,sp<_WebSocketLinker> client) {
    st(GlobalCacheManager)::getInstance()->add(createWebSocketRequestCache(frame->getData(),client));
    auto router = st(WebSocketRouterManager)::getInstance()->getRouter(client->getPath());
    HttpResponseEntity result = router->onInvoke();
    if(result != nullptr) {
        client->sendTextMessage(result->getContent()->get());
    }

    return 0;
}

int _Server::onConnect(sp<_WebSocketLinker> client) {
    //TODO
    return 0;
}

int _Server::onDisconnect(sp<_WebSocketLinker> client) {
    //TODO
    return 0;
}

int _Server::onPong(String,sp<_WebSocketLinker> client) {
    //TODO
    return 0;
}

int _Server::onPing(String,sp<_WebSocketLinker> client) {
    //TODO
    return 0;
}

//http
void _Server::onHttpMessage(int event,HttpLinker client,HttpResponseWriter w,HttpPacket msg) {
    if(event == st(NetEvent)::Message) {
        HashMap<String,String> map = createHashMap<String,String>();
        ServletRequest req = createServletRequest(msg,client);
        //printf("client is %s \n",client->getInetAddress()->toChars());
        ServletRequestCache cache = createServletRequestCache(req,map);
        st(GlobalCacheManager)::getInstance()->add(cache);

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
            

            //st(GlobalCacheManager)::getInstance()->addRequest(req);
            HttpResponseEntity obj = router->invoke();
            
            if(obj != nullptr) {
                entity->setContent(obj->getContent()->get()->toByteArray());
                response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
                response->setEntity(entity);
                w->write(response);
                return;
            }
            //st(GlobalCacheManager)::getInstance()->removeRequest();
        }

        response->getHeader()->setResponseStatus(st(HttpStatus)::BadRequest);
        w->write(response);
    }
}

}
