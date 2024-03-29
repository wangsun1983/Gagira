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
#include "Interceptor.hpp"
#include "HashMap.hpp"
#include "WebSocketServer.hpp"
#include "WebSocketServerBuilder.hpp"
#include "WebSocketListener.hpp"
#include "SqlConfig.hpp"

using namespace obotcha;

namespace gagira  {

DECLARE_CLASS(Server) IMPLEMENTS(HttpListener,WebSocketListener){
public:
    friend class _Controller;

    _Server();
    _Server* setAddress(InetAddress);
    _Server* setWsAddress(InetAddress);
    _Server* addSqlConfig(SqlConfig);
    _Server* setMultiPartFilePath(String);
    _Server* setOption(HttpOption);

    _Server* loadConfigFile(String);

    int start();

    void close();

    void waitForExit(long interval = 0);

    void onHttpMessage(st(Net)::Event event,HttpLinker client,HttpResponseWriter w,HttpPacket msg);
    
    //Websocket callback function
    int onData(WebSocketFrame frame,sp<_WebSocketLinker> client);

    int onConnect(sp<_WebSocketLinker> client);

    int onDisconnect(sp<_WebSocketLinker> client);

    void onPong(String,sp<_WebSocketLinker> client);

    st(WebSocket)::Response onPing(String,sp<_WebSocketLinker> client);

    void addGlobalController(st(HttpMethod)::Id method,ControllerRouter);

    static sp<_Server> getInstance();

    WebSocketServer getWebSocketServer();
    
    void addLazyWsRegistPath(String);
private:
    static sp<_Server> mInstance;

    HttpOption mHttpOption;
    SqlConfig mSqlConfig;

    HttpServer mServer;
    WebSocketServer mWsServer;
    HttpRouterManager mRouterManager;
    HttpResourceManager mResourceManager;

    String mConfigPath;

    HashMap<int,ControllerRouter> mGlobalControllers;
    ArrayList<String> mDelayRegWsPath;
};

}
#endif
