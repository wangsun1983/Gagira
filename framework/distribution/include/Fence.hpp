#ifndef __GAGRIA_FENCE_HPP__
#define __GAGRIA_FENCE_HPP__

#include "Object.hpp"
#include "Mutex.hpp"
#include "DistributeLinker.hpp"
#include "ConcurrentQueue.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(Fence)  {
public:
    _Fence(String);
    String getName();
    String getOwner();
    void setOwner(String);
    void incCount();
    void decCount();
    void setCount(int);
    int getCount();
    bool isFree();
    void reset();
private:
    String mOwner; 
    String mName;
    std::atomic_int mCount;
};

DECLARE_CLASS(ReadFence) IMPLEMENTS(Fence) {
public:
    //void addCount(int);
    _ReadFence(String);
    uint64_t getCount();
    bool isOwner(DistributeLinker);
    bool removeOwner(DistributeLinker);
    void addOwner(DistributeLinker);
    
private:
    //uint64_t count;
    ConcurrentQueue<DistributeLinker> owners;
};

DECLARE_CLASS(WriteFence) IMPLEMENTS(Fence) {
public:
    _WriteFence(String);
};

DECLARE_CLASS(ReadWriteFence) IMPLEMENTS(Fence)  {
public:
    _ReadWriteFence(String);
    ReadFence getReadFence();
    WriteFence getWriteFence();
private:
    ReadFence mReadFence;
    WriteFence mWriteFence;
};

}

#endif
