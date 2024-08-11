#ifndef __GAGRIA_DISTRIBUTE_HANDLER_HPP__
#define __GAGRIA_DISTRIBUTE_HANDLER_HPP__

#include "Object.hpp"
#include "DistributeLinker.hpp"
#include "ArchiveHandleResult.hpp"

using namespace obotcha;

namespace gagira {

class _ArchiveMessage;
class _FenceMessage;

DECLARE_CLASS(DistributeHandler) {
public:
    virtual ArchiveHandleResult onRequest(DistributeLinker,sp<_ArchiveMessage>) {
        return nullptr;
    }

    virtual int onRequest(DistributeLinker,sp<_FenceMessage>) {
        return 0;
    }
};


}

#endif
