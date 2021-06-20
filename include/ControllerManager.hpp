#ifndef __GAGRIA_CONTROLLER_MANAGER_HPP__
#define __GAGRIA_CONTROLLER_MANAGER_HPP__

#include <thread>
#include <mutex>

#include "Object.hpp"
#include "StrongPointer.hpp"
#include "String.hpp"
#include "HashMap.hpp"
#include "Controller.hpp"
#include "Mutex.hpp"

using namespace obotcha;

namespace gagira  {

DECLARE_SIMPLE_CLASS(ControllerManager) {
public:
    static sp<_ControllerManager> getInstance();
    void addController(String,Controller);

private:
    static std::once_flag s_flag;
    static sp<_ControllerManager> mInstance;

    Mutex mControllerMutex;
    HashMap<String,Controller> mControlles;
    _ControllerManager();
    
};

}
#endif
