#include "MqCenterBuilder.hpp"

using namespace obotcha;

namespace gagira {

const int _MqCenterBuilder::DefaultThreadNum = 4;
const int _MqCenterBuilder::DefaultBufferSize = 1024*4;
const int _MqCenterBuilder::DefaultAckTimeout = 1000*60;

_MqCenterBuilder::_MqCenterBuilder() {
    mUrl = nullptr;
    mThreadNum = DefaultThreadNum;
    mBuffSize = DefaultBufferSize;
    mPersistentComp = nullptr;
    mAckTimeout = DefaultAckTimeout;
}

_MqCenterBuilder * _MqCenterBuilder::setUrl(String url) {
    mUrl = url;
    return this;
}

_MqCenterBuilder * _MqCenterBuilder::setThreadNum(int num) {
    mThreadNum = num;
    return this;
}

_MqCenterBuilder * _MqCenterBuilder::setBufferSize(int s) {
    mBuffSize = s;
    return this;
}

_MqCenterBuilder * _MqCenterBuilder::setPersistentComponent(MqPersistentComponent c) {
    mPersistentComp = c;
    return this;
}

_MqCenterBuilder * _MqCenterBuilder::setAckTimeout(int c) {
    mAckTimeout = c;
    return this;
}

MqCenter _MqCenterBuilder::build() {
    return createMqCenter(mUrl,mThreadNum,mBuffSize,mPersistentComp,mAckTimeout);
}

}

