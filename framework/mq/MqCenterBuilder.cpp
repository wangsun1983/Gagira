#include "MqCenterBuilder.hpp"
#include "MqDefaultPersistence.hpp"

using namespace obotcha;

namespace gagira {

const int _MqCenterBuilder::DefaultThreadNum = 4;
const int _MqCenterBuilder::DefaultBufferSize = 1024*4;
const int _MqCenterBuilder::DefaultAckTimeout = 1000;
const int _MqCenterBuilder::DefaultRedeliveryInterval = 1000;
const int _MqCenterBuilder::DefaultRedeliveryTimes = 3;

_MqCenterBuilder::_MqCenterBuilder() {
    mUrl = nullptr;
    mThreadNum = DefaultThreadNum;
    mBuffSize = DefaultBufferSize;
    mPersistence = createMqDefaultPersistence();
    mPersistence->init();
    
    mAckTimeout = DefaultAckTimeout;
    mRedeliveryInterval = DefaultRedeliveryInterval;
    mRedeliveryTimes = DefaultRedeliveryTimes;
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

_MqCenterBuilder * _MqCenterBuilder::setPersistence(MqPersistenceInterface c) {
    mPersistence = c;
    return this;
}

_MqCenterBuilder * _MqCenterBuilder::setAckTimeout(int c) {
    mAckTimeout = c;
    return this;
}

_MqCenterBuilder * _MqCenterBuilder::setRedeliveryInterval(int c) {
    mRedeliveryInterval = c;
    return this;
}

_MqCenterBuilder * _MqCenterBuilder::setRedeliveryTimes(int c) {
    mRedeliveryTimes = c;
    return this;
}

MqCenter _MqCenterBuilder::build() {
    return createMqCenter(mUrl,mThreadNum,mBuffSize,mPersistence,mAckTimeout,mRedeliveryTimes,mRedeliveryInterval);
}

}

