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
#include "Enviroment.hpp"
#include "ForEveryOne.hpp"

using namespace obotcha;

namespace gagira  {

sp<_Server> _Server::mInstance = nullptr;

_Server::_Server() {
    mServer = nullptr;
    mRouterManager = st(HttpRouterManager)::getInstance();
    mResourceManager = st(HttpResourceManager)::getInstance();
    mInstance = AutoClone(this);
    mDelayRegWsPath = createArrayList<String>();
    mGlobalControllers = createHashMap<int,ControllerRouter>();
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

_Server* _Server::setMultiPartFilePath(String path) {
    st(Enviroment)::getInstance()->set(st(Enviroment)::gHttpMultiPartFilePath,path);
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

void _Server::addLazyWsRegistPath(String path) {
    mDelayRegWsPath->add(path);
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
    if(ws_port != -1) {
        auto webSocketBuilder = createWebSocketServerBuilder();
        InetAddress address;
        if(ws_ip != nullptr && ws_ip->contains(":")) {
            address = createInet6Address(ws_ip,ws_port);
        } else {
            address = createInet4Address(ws_ip,ws_port);
        }

        webSocketBuilder->setInetAddr(address);
        mWsServer = webSocketBuilder->build();
        
        ForEveryOne(path,mDelayRegWsPath) {
            mWsServer->bind(path, st(Server)::getInstance());             
        }
        mDelayRegWsPath->clear();
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

void _Server::addGlobalController(int method,ControllerRouter r) {
    mGlobalControllers->put(method,r);
    /*
    ArrayList<Interceptor> list = mGlobalInterceptors->get(createInteger(method));
    if(list == nullptr) {
        list = createArrayList<Interceptor>();
        mGlobalInterceptors->put(createInteger(method),list);
    }
    list->add(c); */
}

//websocket
int _Server::onData(WebSocketFrame frame,sp<_WebSocketLinker> client) {
    st(GlobalCacheManager)::getInstance()->add(createWebSocketRequestCache(frame->getData(),client));
    auto router = st(WebSocketRouterManager)::getInstance()->getRouter(client->getPath());
    WsResponseEntity result = router->onInvoke();
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
        //remove thread local cache first
        st(GlobalCacheManager)::getInstance()->remove();

        ServletRequest req = createServletRequest(msg,client);
        int method = msg->getHeader()->getMethod();
        HttpResponse response = createHttpResponse();

        auto gController = mGlobalControllers->get(method);
        if(gController != nullptr) {
            st(GlobalCacheManager)::getInstance()->add(createHttpPacketCache(msg));
            auto entity = gController->onInvoke();
            if(entity != nullptr && entity->getStatus() != st(HttpStatus)::Ok) {
                response->getHeader()->setResponseStatus(entity->getStatus());
                w->write(response);
                return;
            }
        }

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

        FetchRet(router,map) = mRouterManager->getRouter(method,url);
        ServletRequestCache cache = createServletRequestCache(req,createControllerParam(map));
        st(GlobalCacheManager)::getInstance()->add(cache);
        if(router != nullptr) {
            HttpEntity entity = createHttpEntity();
            HttpResponseEntity obj = router->invoke();
            if(obj != nullptr) {
                if(obj->getStatus() != st(HttpResponseEntity)::NoResponse) {
                    auto content = obj->getContent();
                    if(content != nullptr) {
                        entity->setContent(content->get()->toByteArray());
                    }
                    entity->setChunk(obj->getChunk());

                    response->getHeader()->setResponseStatus(obj->getStatus());
                    response->setEntity(entity);
                    w->write(response);
                }
                return;
            }
        }

        response->getHeader()->setResponseStatus(st(HttpStatus)::BadRequest);
        w->write(response);
    }
}

}
