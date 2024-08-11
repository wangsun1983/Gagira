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
        Idle = 0,
        ApplyFence,
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
    
    _FenceMessage();
    _FenceMessage(st(FenceMessage)::Type event,String name,uint64_t waittime = 0);

    uint64_t getWaitTime();
    uint64_t getResult();
    String getFenceName();
    st(FenceMessage)::Type getEvent();

protected:
    uint32_t event;
    String fencename;
    // uint32_t result;
    // uint64_t waittime
    uint64_t privateData;

    DECLARE_REFLECT_FIELD(FenceMessage,event,fencename,privateData);
};

DECLARE_CLASS(ConfirmFenceMessage) IMPLEMENTS(FenceMessage) {
public:
    _ConfirmFenceMessage();
    _ConfirmFenceMessage(String name,uint64_t result = 0);
};


}

#endif
