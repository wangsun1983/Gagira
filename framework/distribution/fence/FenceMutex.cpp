#include "FenceMutex.hpp"

namespace gagira {

//--------- FenceMutex ------------
_FenceMutex::_FenceMutex(FenceConnection c,String n) {
    mConn = c;
    mName = n;
}

int _FenceMutex::lock(long interval) {
    return mConn->acquireFence(mName,interval);    
}

int _FenceMutex::_FenceMutex::unlock() {
    return mConn->releaseFence(mName);
}

//--------- FenceReadWriteLock -----------
_FenceReadWriteLock::_FenceReadWriteLock(FenceConnection c,String name) {
    mConn = c;
    mName = name;
}

sp<_FenceReadLock> _FenceReadWriteLock::getReadLock() {
    return FenceReadLock::New(mConn,mName);
}

sp<_FenceWriteLock> _FenceReadWriteLock::getWriteLock() {
    return FenceWriteLock::New(mConn,mName);
}

//----------- FenceReadLock -----------
_FenceReadLock::_FenceReadLock(FenceConnection c,String name) {
    mConn = c;
    mName = name;
}

int _FenceReadLock::lock(long interval) {
    return mConn->acquireReadFence(mName,interval);
}

int _FenceReadLock::unlock() {
    return mConn->releaseReadFence(mName);
}

//----------- FenceWriteLock -----------
_FenceWriteLock::_FenceWriteLock(FenceConnection c,String name) {
    mConn = c;
    mName = name;
}

int _FenceWriteLock::lock(long interval) {
    return mConn->acquireWriteFence(mName,interval);
}

int _FenceWriteLock::unlock() {
    return mConn->releaseWriteFence(mName);
}

}
