#include "GlobalCacheManager.hpp"
#include "ControllerParam.hpp"

using namespace obotcha;

namespace gagira  {

sp<_GlobalCacheManager> _GlobalCacheManager::mInstance = nullptr;

//ServletRequestCache
_ServletRequestCache::_ServletRequestCache(ServletRequest request,sp<_ControllerParam> param) {
    r = request;
    p = param;
}

//WebsocketMsgCache
_WebSocketRequestCache::_WebSocketRequestCache(ByteArray data,WebSocketLinker c) {
    msg = data;
    client = c;
}

//
_GlobalCacheManager::_GlobalCacheManager() {
    mCaches = createThreadLocal<BaseServerCache>();
}

sp<_GlobalCacheManager> _GlobalCacheManager::getInstance() {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        _GlobalCacheManager *p = new _GlobalCacheManager();
        p->mInstance.set_pointer(p);
    });

    return mInstance;
}

void _GlobalCacheManager::add(BaseServerCache c) {
    mCaches->set(c);
}

ServletRequest _GlobalCacheManager::getRequest() {
    ServletRequestCache cache = Cast<ServletRequestCache>(mCaches->get());
    return cache->r;
}

ControllerParam _GlobalCacheManager::getParam() {
    ServletRequestCache cache = Cast<ServletRequestCache>(mCaches->get());
    return cache->p;
}

ByteArray _GlobalCacheManager::getWsRequest() {
    WebSocketRequestCache cache = Cast<WebSocketRequestCache>(mCaches->get());
    return cache->msg;
}

WebSocketLinker _GlobalCacheManager::getWsClient() {
    WebSocketRequestCache cache = Cast<WebSocketRequestCache>(mCaches->get());
    return cache->client;
}

void _GlobalCacheManager::remove() {
    mCaches->remove();
}

}
