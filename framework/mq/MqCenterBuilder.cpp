#include "MqCenterBuilder.hpp"

using namespace obotcha;

namespace gagira {


_MqCenterBuilder::_MqCenterBuilder() {
    mUrl = nullptr;
    mOption = nullptr;
}

_MqCenterBuilder * _MqCenterBuilder::setUrl(String url) {
    mUrl = url;
    return this;
}

_MqCenterBuilder * _MqCenterBuilder::setOption(MqOption option) {
    mOption = option;
    return this;
}

MqCenter _MqCenterBuilder::build() {
    return createMqCenter(mUrl,mOption);
}

}

