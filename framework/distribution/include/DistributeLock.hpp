#ifndef __GAGRIA_DISTRIBUTE_LOCK_HPP__
#define __GAGRIA_DISTRIBUTE_LOCK_HPP__

#include "Object.hpp"
#include "Lock.hpp"
#include "FenceConnection.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DistributeLock) IMPLEMENTS(Lock) {
public:
    _DistributeLock(String name,String url);
    int lock(long interval = 0);
    int unlock();
private:
    Mutex mMutex;
    FenceConnection mConnection;
    String mName;
};

}

#endif
