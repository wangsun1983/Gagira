#include "ServletRequestManager.hpp"

using namespace obotcha;

namespace gagira  {

std::once_flag _ServletRequestManager::s_flag;
sp<_ServletRequestManager> _ServletRequestManager::mInstance;

_ServletRequestManager::_ServletRequestManager() {
    mRequests = createThreadLocal<ServletRequest>();
}

sp<_ServletRequestManager> _ServletRequestManager::getInstance() {
    std::call_once(s_flag, [&]() {
        _ServletRequestManager *p = new _ServletRequestManager();
        p->mInstance.set_pointer(p);
    });

    return mInstance;
}

void _ServletRequestManager::addRequest(ServletRequest r) {
    mRequests->set(r);
}

void _ServletRequestManager::removeRequest() {
    mRequests->remove();
}

ServletRequest _ServletRequestManager::getRequest() {
    return mRequests->get();
}

}
