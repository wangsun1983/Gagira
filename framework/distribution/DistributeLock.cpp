#include "DistributeLock.hpp"
#include "AutoLock.hpp"
#include "TimeWatcher.hpp"
#include "InitializeException.hpp"

namespace gagira {

_DistributeLock::_DistributeLock(String name,String url) {
    mConnection = FenceConnection::New(url);
    Panic(mConnection->connect() != 0,InitializeException,"Fail to connect server");
    mMutex = Mutex::New(name);
    mName = name;
}

int _DistributeLock::lock(long interval) {
    TimeWatcher watcher = TimeWatcher::New();
    watcher->start();
    int result = mMutex->lock(interval);
    auto cost = watcher->stop();

    if(result < 0 || cost >= interval) {
        return -1;
    }

    return mConnection->acquireFence(mName,interval - cost);
}

int _DistributeLock::unlock() {
    mMutex->unlock();
    return mConnection->releaseFence(mName);
}

}

