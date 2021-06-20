#include "ControllerManager.hpp"
#include "AutoLock.hpp"

namespace gagira  {

sp<_ControllerManager> _ControllerManager::getInstance() {
    std::call_once(s_flag, [&]() {
        _ControllerManager *p = new _ControllerManager();
        p->mInstance.set_pointer(p);
    });

    return mInstance;
}

void _ControllerManager::addController(String,Controller) {
    AutoLock l(mControllerMutex);
    
}


}
