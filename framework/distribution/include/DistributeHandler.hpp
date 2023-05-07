#ifndef __GAGRIA_DISTRIBUTE_HANDLER_HPP__
#define __GAGRIA_DISTRIBUTE_HANDLER_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "DistributeLinker.hpp"
#include "QueueMessage.hpp"
#include "SpaceMessage.hpp"
#include "ArchiveHandleResult.hpp"

using namespace obotcha;

namespace gagira {

class _ArchiveMessage;

DECLARE_CLASS(DistributeHandler) {
public:
    enum Type {
        Deal = 0,
        Drop
    };

    virtual ArchiveHandleResult onRequest(DistributeLinker,sp<_ArchiveMessage>) {
        return nullptr;
    }
};


}

#endif
