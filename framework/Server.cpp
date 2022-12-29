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
    st(Configs)::getInstance()->setHttpServerAddress(addr->getAddress());
    st(Configs)::getInstance()->setHttpServerPort(addr->getPort());
    return this;
}

_Server* _Server::setOption(HttpOption option) {
    mHttpOption = option;
    return this;
}

_Server* _Server::setWsAddress(InetAddress addr) {
    st(Configs)::getInstance()->setWebSocketServerAddress(addr->getAddress());
    st(Configs)::getInstance()->setWebSocketServerPort(addr->getPort());
    return this;
}

_Server* _Server::addSqlConfig(SqlConfig config) {
   st(Configs)::getInstance()->addSqlConfig(config);
   return this;
}

_Server* _Server::loadConfigFile(String path) {
    st(Configs)::getInstance()->load(createFile(path));   
    return this;
}

int _Server::start() {
    //init http server
    String ip = st(Configs)::getInstance()->getHttpServerAddress();
    int port = st(Configs)::getInstance()->getHttpServerPort();

    InetAddress address;
    if(ip == nullptr || ip->contains(".")) {
        address = createInet4Address(ip,port);
    } else {
        address = createInet6Address(ip,port);
    }
    auto httpBuilder = createHttpServerBuilder();
    httpBuilder->setAddress(address);
    httpBuilder->setListener(AutoClone(this));
    mServer = httpBuilder->build();

    //init websocket server
    String ws_ip = st(Configs)::getInstance()->getWebSocketServerAddress();
    int ws_port = st(Configs)::getInstance()->getWebSocketServerPort();

    if(ws_ip != nullptr && ws_port != -1) {
        auto webSocketBuilder = createWebSocketServerBuilder();
        InetAddress address;
        if(ws_ip->contains(":")) {
            address = createInet6Address(ws_ip,ws_port);
        } else {
            address = createInet4Address(ws_ip,ws_port);
        }

        webSocketBuilder->setInetAddr(address);
        mWsServer = webSocketBuilder->build();
    }

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
    mServer->waitForExit(interval);
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

void _Server::onPong(String,sp<_WebSocketLinker> client) {
    //TODO
    //return true;
}

int _Server::onPing(String,sp<_WebSocketLinker> client) {
    //TODO
    return AutoResponse;
}

//http
void _Server::onHttpMessage(int event,HttpLinker client,HttpResponseWriter w,HttpPacket msg) {
    if(event == st(NetEvent)::Message) {
        ServletRequest req = createServletRequest(msg,client);
        //printf("client is %s \n",client->getInetAddress()->toChars());
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
        if(file != nullptr) {
            response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
            //response->setFile(file);
            HttpChunk chunk = createHttpChunk(file);
            response->getEntity()->setChunk(chunk);
            w->write(response);
            return;
        }

        HttpRouter router;
        HashMap<String,String> map;
        FetchRet(router,map) = mRouterManager->getRouter(method,url);
        ServletRequestCache cache = createServletRequestCache(req,createControllerParam(map));
        st(GlobalCacheManager)::getInstance()->add(cache);
        printf("server trace1 \n");
        if(router != nullptr) {
            printf("server trace2 \n");
            //TODO?
            HttpEntity entity = createHttpEntity();
            //st(GlobalCacheManager)::getInstance()->addRequest(req);
            HttpResponseEntity obj = router->invoke();
            if(obj != nullptr) {
                if(obj->getStatus() != st(HttpResponseEntity)::NoResponse) {
                    entity->setContent(obj->getContent()->get()->toByteArray());
                    response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
                    response->setEntity(entity);
                    w->write(response);
                }
                return;
            }
            //st(GlobalCacheManager)::getInstance()->removeRequest();
        }

        response->getHeader()->setResponseStatus(st(HttpStatus)::BadRequest);
        w->write(response);
    }
}

}
