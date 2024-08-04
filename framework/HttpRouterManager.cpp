#include "HttpRouterManager.hpp"

namespace gagira {

sp<_HttpRouterManager> _HttpRouterManager::mInstance;

HttpRouterManager _HttpRouterManager::getInstance() {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        _HttpRouterManager *p = new _HttpRouterManager();
        p->mInstance.set_pointer(p);
    });

    return mInstance;
}

_HttpRouterManager::_HttpRouterManager() {
    for (int i = 0; i < static_cast<int>(st(HttpMethod)::Id::Max); i++) {
        mMaps[i] = HttpRouterMap::New();
    }
}

void _HttpRouterManager::addRouter(int method, HttpRouter r) {
    mMaps[method]->addRouter(r);
}

DefRet(HttpRouter,HashMap<String,String>) _HttpRouterManager::getRouter(int method, String url) {
    return mMaps[method]->findRouter(url);
}

}
