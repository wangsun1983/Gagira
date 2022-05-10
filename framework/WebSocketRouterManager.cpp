#include <mutex>

#include "WebSocketRouterManager.hpp"

namespace gagira {

sp<_WebSocketRouterManager> _WebSocketRouterManager::mInstance = nullptr;

_WebSocketRouterManager::_WebSocketRouterManager() {
    lock = createReadWriteLock();
    mReadLock = lock->getReadLock();
    mWriteLock = lock->getWriteLock();
    mControllers = createHashMap<String,ControllerRouter>();
}

sp<_WebSocketRouterManager> _WebSocketRouterManager::getInstance() {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        _WebSocketRouterManager *p = new _WebSocketRouterManager();
        p->mInstance.set_pointer(p);
    });

    return mInstance;
}

void _WebSocketRouterManager::addRouter(String path,ControllerRouter c) {
    AutoLock l(mWriteLock);
    mControllers->put(path,c);
}

ControllerRouter _WebSocketRouterManager::getRouter(String path) {
    AutoLock l(mReadLock);
    return mControllers->get(path);
}

}