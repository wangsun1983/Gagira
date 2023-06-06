#include "DistributeReadWriteLock.hpp"
#include "Exception.hpp"
#include "InitializeException.hpp"
#include "TimeWatcher.hpp"

using namespace obotcha;

namespace gagira {

//DistributeReadLock
_DistributeReadLock::_DistributeReadLock(String name,ReadLock rdlock,FenceConnection c) {
    mName = name;
    mReadLock = rdlock;
    mConnection = c;
}
    
int _DistributeReadLock::lock(long interval) {
    TimeWatcher watcher = createTimeWatcher();
    int result = mReadLock->lock(interval);
    auto cost = watcher->stop();

    if(result < 0 || cost >= interval) {
        return -1;
    }

    return mConnection->acquireReadFence(mName,interval - cost);
}

int _DistributeReadLock::unlock() {
    mReadLock->unlock();
    return mConnection->releaseReadFence(mName);
}

//DistributeWriteLock
_DistributeWriteLock::_DistributeWriteLock(String name,WriteLock wrlock,FenceConnection c) {
    mName = name;
    mWriteLock = wrlock;
    mConnection = c;
}

int _DistributeWriteLock::_DistributeWriteLock::lock(long interval) {
    TimeWatcher watcher = createTimeWatcher();
    int result = mWriteLock->lock(interval);
    auto cost = watcher->stop();

    if(result < 0 || cost >= interval) {
        return -1;
    }

    return mConnection->acquireWriteFence(mName,interval - cost);
}
    
int _DistributeWriteLock::unlock() {
    mWriteLock->unlock();
    return mConnection->releaseWriteFence(mName);
}

//DistributeReadWriteLock
_DistributeReadWriteLock::_DistributeReadWriteLock(String name,String url) {
    mReadWriteLock = createReadWriteLock();
    mConnection = createFenceConnection(url);
    Panic(mConnection->connect() < 0,InitializeException,"connect failed");
}

DistributeReadLock _DistributeReadWriteLock::getReadLock() {
    return createDistributeReadLock(mName,mReadWriteLock->getReadLock(),mConnection);
}

DistributeWriteLock _DistributeReadWriteLock::getWriteLock() {
    return createDistributeWriteLock(mName,mReadWriteLock->getWriteLock(),mConnection);
}

}

