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
    return BroadcastCenter::New(mUrl,mOption);
}

SpaceCenter _DistributeCenterBuilder::buildSpaceCenter() {
    return SpaceCenter::New(mUrl,mOption);
}

QueueCenter _DistributeCenterBuilder::buildQueueCenter() {
    return QueueCenter::New(mUrl,mOption);
}

ArchiveCenter _DistributeCenterBuilder::buildArchiveCenter() {
    return ArchiveCenter::New(mUrl,Cast<ArchiveOption>(mOption));
}

}

