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

MqCenter _DistributeCenterBuilder::buildMqCenter() {
    return createMqCenter(mUrl,mOption);
}

SpaceCenter _DistributeCenterBuilder::buildSpaceCenter() {
    return createSpaceCenter(mUrl,mOption);
}

}

