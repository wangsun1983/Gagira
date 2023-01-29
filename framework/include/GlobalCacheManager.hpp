#ifndef __GAGRIA_HTTP_SERVLET_REQUEST_MANAGER_HPP__
#define __GAGRIA_HTTP_SERVLET_REQUEST_MANAGER_HPP__

#include <thread>

#include "Object.hpp"
#include "StrongPointer.hpp"
#include "Controller.hpp"
#include "ThreadLocal.hpp"
#include "ControllerParam.hpp"
#include "ServletRequest.hpp"
#include "WebSocketLinker.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(BaseServerCache) {

};

DECLARE_CLASS(ServletRequestCache) IMPLEMENTS(BaseServerCache) {
public:
    _ServletRequestCache(ServletRequest,sp<_ControllerParam>);
    ServletRequest r;
    sp<_ControllerParam> p;
};

DECLARE_CLASS(WebSocketRequestCache) IMPLEMENTS(BaseServerCache){
public:
    _WebSocketRequestCache(ByteArray,WebSocketLinker);
    WebSocketLinker client;
    ByteArray msg;
};

DECLARE_CLASS(HttpPacketCache) IMPLEMENTS(BaseServerCache){
public:
    _HttpPacketCache(HttpPacket);
    HttpPacket mPacket;
};

DECLARE_CLASS(GlobalCacheManager) {
public:
    static sp<_GlobalCacheManager> getInstance();

    void add(BaseServerCache);

    ServletRequest getRequest();
    HttpPacket getPacket();
    ControllerParam getParam();

    ByteArray getWsRequest();
    WebSocketLinker getWsClient();
    void remove();

private:
    _GlobalCacheManager();
    static sp<_GlobalCacheManager> mInstance;

    ThreadLocal<BaseServerCache> mCaches;
};

}
#endif
