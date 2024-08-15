#ifndef __GAGRIA_FENCE_CLIENT_HPP__
#define __GAGRIA_FENCE_CLIENT_HPP__

#include "Object.hpp"
#include "FenceMutex.hpp"
#include "String.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(FenceFactory) {
public:
    _FenceFactory(String);
    FenceMutex getFenceMutex(String);
    FenceReadWriteLock getFenceReadWriteLock(String);

private:
    String mUrl;
};


}

#endif
