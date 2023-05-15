#ifndef __GAGRIA_FENCE_MESSAGE_HPP__
#define __GAGRIA_FENCE_MESSAGE_HPP__

#include "Object.hpp"
#include "Serializable.hpp"
#include "ByteArray.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(FenceMessage) IMPLEMENTS(Serializable) {
public:
    enum Type {
        ApplyFence = 1,
        ConfirmFence,

        ApplyReleaseFence,
        ConfirmReleaseFence,

        ApplyReadFence,
        ConfirmReadFence,

        ApplyReleaseReadFence,
        ConfirmReleaseReadFence,

        ApplyWriteFence,
        ConfirmWriteFence,

        ApplyReleaseWriteFence,
        ConfirmReleaseWriteFence,
    };
    
    uint32_t event;
    String fencename;
    // uint32_t result;
    // uint64_t waittime
    uint64_t privateData;

    _FenceMessage();
    _FenceMessage(int event,String name,uint64_t waittime = 0);

    uint64_t getWaitTime();
    uint64_t getResult();

    DECLARE_REFLECT_FIELD(FenceMessage,event,fencename,privateData);
};

DECLARE_CLASS(ConfirmFenceMessage) IMPLEMENTS(FenceMessage) {
public:
    _ConfirmFenceMessage();
    _ConfirmFenceMessage(String name,uint64_t result = 0);
};


}

#endif
