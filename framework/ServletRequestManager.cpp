#include "ServletRequestManager.hpp"
#include "ControllerParam.hpp"

using namespace obotcha;

namespace gagira  {

sp<_ServletRequestManager> _ServletRequestManager::mInstance;

_ServletRequestCache::_ServletRequestCache(ServletRequest request,sp<_ControllerParam> param) {
    r = request;
    p = param;
}

_ServletRequestManager::_ServletRequestManager() {
    mCaches = createThreadLocal<ServletRequestCache>();
}

sp<_ServletRequestManager> _ServletRequestManager::getInstance() {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        _ServletRequestManager *p = new _ServletRequestManager();
        p->mInstance.set_pointer(p);
    });

    return mInstance;
}

void _ServletRequestManager::add(ServletRequestCache c) {
    mCaches->set(c);
}

ServletRequest _ServletRequestManager::getRequest() {
    return mCaches->get()->r;
}

ControllerParam _ServletRequestManager::getParam() {
    return mCaches->get()->p;
}

void _ServletRequestManager::remove() {
    mCaches->remove();
}

}
