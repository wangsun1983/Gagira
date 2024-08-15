#ifndef __GAGRIA_FENCE_MUTEX_HPP__
#define __GAGRIA_FENCE_MUTEX_HPP__

#include "Object.hpp"
#include "Lock.hpp"
#include "FenceConnection.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(FenceMutex) IMPLEMENTS(Lock) {
public:
    _FenceMutex(FenceConnection,String);
    int lock(long interval = st(Concurrent)::kWaitForEver);
    int unlock();

private:
    FenceConnection mConn;
    String mName;
};

DECLARE_CLASS(FenceReadLock) IMPLEMENTS(Lock) {
public:
    _FenceReadLock(FenceConnection,String);
    int lock(long interval = st(Concurrent)::kWaitForEver);
    int unlock();

private:
    FenceConnection mConn;
    String mName;
};

DECLARE_CLASS(FenceWriteLock) IMPLEMENTS(Lock) {
public:
    _FenceWriteLock(FenceConnection,String);
    int lock(long interval = st(Concurrent)::kWaitForEver);
    int unlock();

private:
    FenceConnection mConn;
    String mName;
};

DECLARE_CLASS(FenceReadWriteLock) {
public:
    _FenceReadWriteLock(FenceConnection,String);
    sp<_FenceReadLock> getReadLock();
    sp<_FenceWriteLock> getWriteLock();

private:
    FenceConnection mConn;
    String mName;
};


}

#endif
