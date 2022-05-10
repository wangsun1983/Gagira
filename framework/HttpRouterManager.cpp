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
    for (int i = 0; i < st(HttpMethod)::Max; i++) {
        mMaps[i] = createHttpRouterMap();
    }
}

void _HttpRouterManager::addRouter(int method, HttpRouter r) {
    printf("addRouter method is %d \n",method);
    mMaps[method]->addRouter(r);
}

HttpRouter _HttpRouterManager::getRouter(int method, String url,
                                         HashMap<String, String> &result) {
    return mMaps[method]->findRouter(url, result);
}

}
