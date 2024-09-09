#include "Fence.hpp"
#include "ForEveryOne.hpp"
#include "Log.hpp"

namespace gagira {

//Fence
_Fence::_Fence(String name) {
    mName = name;
    mCount = 0;
    mFlag = FenceMutex;
}

String _Fence::getName() {
    return mName;
}

String _Fence::getOwner() {
    return mOwner;
}

void _Fence::setOwner(String token) {
    mOwner = token;
}

void _Fence::incCount() {
    mCount++;
}

void _Fence::decCount() {
    mCount--;
}

int _Fence::getCount() {
    return mCount;
}

void _Fence::setCount(int c) {
    mCount = c;
}

bool _Fence::isFree() {
    if(mCount < 0) {
        LOG(ERROR)<<"Fence:"<<mName->toChars()<<" count is "<<mCount;
    }
    return mCount <= 0;
}

void _Fence::reset() {
    mOwner = nullptr;
    mCount = 0;
}

int _Fence::getType() {
    return mFlag;
}

//ReadFence
_ReadFence::_ReadFence(String name):_Fence(name) {
    owners = ConcurrentQueue<DistributeLinker>::New();
    mFlag = ReadFence;
}

uint64_t _ReadFence::getCount() {
    return owners->size();
}

void _ReadFence::addOwner(DistributeLinker l) {
    owners->add(l);
}

bool _ReadFence::isOwner(DistributeLinker linker) {
    ForEveryOne(l,owners) {
        if(l == linker) {
            return true;
        }
    }

    return false;
}

bool _ReadFence::removeOwner(DistributeLinker linker) {
    return owners->remove(linker) >= 0;
}

//ReadWriteFence
_ReadWriteFence::_ReadWriteFence(String name):_Fence(name) {
    mReadFence = ReadFence::New(name);
    mWriteFence = WriteFence::New(name);
    mFlag = (_Fence::Type::ReadFence | _Fence::Type::WriteFence);
}

ReadFence _ReadWriteFence::getReadFence() {
    return mReadFence;
}

WriteFence _ReadWriteFence::getWriteFence() {
    return mWriteFence;
}

//
_WriteFence::_WriteFence(String name):_Fence(name) {
    mFlag = WriteFence;
}

}
