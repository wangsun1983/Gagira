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
#include "Configs.hpp"
#include "Inet4Address.hpp"
#include "Inet6Address.hpp"
#include "ForEveryOne.hpp"
#include "WsResponseEntity.hpp"

using namespace obotcha;

namespace gagira  {

sp<_Server> _Server::mInstance = nullptr;

_Server::_Server() {
    mServer = nullptr;
    mRouterManager = st(HttpRouterManager)::getInstance();
    mResourceManager = st(HttpResourceManager)::getInstance();
    mInstance = AutoClone(this);
    mDelayRegWsPath = ArrayList<String>::New();
    mGlobalControllers = HashMap<int,ControllerRouter>::New();
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
    //st(Enviroment)::getInstance()->set(st(Enviroment)::gHttpMultiPartFilePath,path);
    //TODO?
    return this;
}

_Server* _Server::addSqlConfig(SqlConfig config) {
   st(Configs)::getInstance()->addSqlConfig(config);
   return this;
}

_Server* _Server::loadConfigFile(String path) {
    st(Configs)::getInstance()->load(File::New(path));   
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
        address = Inet4Address::New(ip,port);
    } else {
        address = Inet6Address::New(ip,port);
    }
    auto httpBuilder = HttpServerBuilder::New();
    httpBuilder->setAddress(address);
    httpBuilder->setListener(AutoClone(this));
    mServer = httpBuilder->build();

    //init websocket server
    String ws_ip = st(Configs)::getInstance()->getWebSocketServerAddress();
    int ws_port = st(Configs)::getInstance()->getWebSocketServerPort();
    if(ws_port != -1) {
        auto webSocketBuilder = WebSocketServerBuilder::New();
        InetAddress wsAddress;
        if(ws_ip != nullptr && ws_ip->contains(":")) {
            wsAddress = Inet6Address::New(ws_ip,ws_port);
        } else {
            wsAddress = Inet4Address::New(ws_ip,ws_port);
        }

        webSocketBuilder->setInetAddr(wsAddress);
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

void _Server::addGlobalController(st(HttpMethod)::Id method,ControllerRouter r) {
    mGlobalControllers->put(static_cast<int>(method),r);
    /*
    ArrayList<Interceptor> list = mGlobalInterceptors->get(Integer::New(method));
    if(list == nullptr) {
        list = ArrayList<Interceptor>::New();
        mGlobalInterceptors->put(Integer::New(method),list);
    }
    list->add(c); */
}

//websocket
int _Server::onData(WebSocketFrame frame,sp<_WebSocketLinker> client) {
    st(GlobalCacheManager)::getInstance()->add(WebSocketRequestCache::New(frame->getData(),client));
    auto router = st(WebSocketRouterManager)::getInstance()->getRouter(client->getPath());
    WsResponseEntity result = Cast<WsResponseEntity>(router->onInvoke());
    if(result != nullptr) {
        auto binaryData = result->getBinaryData();
        if(binaryData != nullptr) {
            client->sendBinaryMessage(binaryData);
            return 0;
        }

        auto textData = result->getTextData();
        if(textData != nullptr) {
            client->sendTextMessage(textData);
            return 0;
        }
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

st(WebSocket)::Response _Server::onPing(String,sp<_WebSocketLinker> client) {
    //TODO
    return st(WebSocket)::Response::Auto;
}

//http
void _Server::onHttpMessage(st(Net)::Event event,HttpLinker client,HttpResponseWriter w,HttpPacket msg) {
    if(event == st(Net)::Event::Message) {
        //remove thread local cache first
        st(GlobalCacheManager)::getInstance()->remove();

        ServletRequest req = ServletRequest::New(msg,client);
        int method = static_cast<int>(msg->getHeader()->getMethod());
        HttpResponse response = HttpResponse::New();

        auto gController = mGlobalControllers->get(method);
        if(gController != nullptr) {
            st(GlobalCacheManager)::getInstance()->add(HttpPacketCache::New(msg));
            auto entity = gController->onInvoke();
            if(entity != nullptr) {
                auto httpEntity = Cast<HttpResponseEntity>(entity);
                if(httpEntity != nullptr) {
                    response->getHeader()->setResponseStatus(httpEntity->getStatus());
                    w->write(response);
                    return;
                }
            }
        }

        String url = msg->getHeader()->getUrl()->getRawUrl();
        File file = mResourceManager->findResource(url);
        if(file != nullptr) {
            response->getHeader()->setResponseStatus(st(HttpStatus)::Ok);
            HttpChunk chunk = HttpChunk::New(file);
            response->getEntity()->setChunk(chunk);
            w->write(response);
            return;
        }
        
        FetchRet(router,map) = mRouterManager->getRouter(method,url);
        ServletRequestCache cache = ServletRequestCache::New(req,ControllerParam::New(map));
        st(GlobalCacheManager)::getInstance()->add(cache);
        if(router != nullptr) {
            HttpEntity entity = HttpEntity::New();
            ResponseEntity obj = router->invoke();
            if(obj != nullptr) {
                HttpResponseEntity responseEntity = Cast<HttpResponseEntity>(obj);
                if(responseEntity->getStatus() != st(HttpResponseEntity)::NoResponse) {
                    auto content = responseEntity->getContent();
                    if(content != nullptr) {
                        entity->setContent(content->get()->toByteArray());
                    }
                    entity->setChunk(responseEntity->getChunk());

                    response->getHeader()->setResponseStatus(responseEntity->getStatus());
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
