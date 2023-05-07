#include "DistributeCenterBuilder.hpp"

using namespace obotcha;

namespace gagira {


_DistributeCenterBuilder::_DistributeCenterBuilder() {
    mUrl = nullptr;
    mOption = nullptr;
}

_DistributeCenterBuilder * _DistributeCenterBuilder::setUrl(String url) {
    mUrl = url;
    return this;
}

_DistributeCenterBuilder * _DistributeCenterBuilder::setOption(DistributeOption option) {
    mOption = option;
    return this;
}

BroadcastCenter _DistributeCenterBuilder::buildBroadcastCenter() {
    return createBroadcastCenter(mUrl,mOption);
}

SpaceCenter _DistributeCenterBuilder::buildSpaceCenter() {
    return createSpaceCenter(mUrl,mOption);
}

QueueCenter _DistributeCenterBuilder::buildQueueCenter() {
    return createQueueCenter(mUrl,mOption);
}

ArchiveCenter _DistributeCenterBuilder::buildArchiveCenter() {
    return createArchiveCenter(mUrl,Cast<ArchiveOption>(mOption));
}

}

