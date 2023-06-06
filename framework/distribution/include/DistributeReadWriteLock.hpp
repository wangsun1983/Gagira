#ifndef __GAGRIA_DISTRIBUTE_READ_WRITE_LOCK_HPP__
#define __GAGRIA_DISTRIBUTE_READ_WRITE_LOCK_HPP__

#include "Object.hpp"
#include "Lock.hpp"
#include "FenceConnection.hpp"
#include "ReadWriteLock.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DistributeReadLock) IMPLEMENTS(Lock) {
public:
    _DistributeReadLock(String name,ReadLock,FenceConnection);
    int lock(long interval = 0);
    int unlock();

private:
    String mName;
    ReadLock mReadLock;
    FenceConnection mConnection;
};

DECLARE_CLASS(DistributeWriteLock) IMPLEMENTS(Lock) {
public:
    _DistributeWriteLock(String name,WriteLock,FenceConnection);
    int lock(long interval = 0);
    int unlock();

private:
    String mName;
    WriteLock mWriteLock;
    FenceConnection mConnection;
};

DECLARE_CLASS(DistributeReadWriteLock) {
public:
    _DistributeReadWriteLock(String name,String url);
    DistributeReadLock getReadLock();
    DistributeWriteLock getWriteLock();

private:
    String mName;
    ReadWriteLock mReadWriteLock;
    FenceConnection mConnection;
};

}

#endif
