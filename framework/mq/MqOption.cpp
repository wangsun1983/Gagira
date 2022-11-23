#include "MqOption.hpp"

using namespace obotcha;

namespace gagira {

const int _MqOption::DefaultClientRecvBuffSize = 4096*4;
const int _MqOption::DefaultThreadNum = 4;
const int _MqOption::DefaultBufferSize = 1024*4;
const long _MqOption::DefaultAckTimeout = 1000;
const int _MqOption::DefaultReDeliveryInterval = 1000;
const int _MqOption::DefaultReDeliveryTimes = 3;

_MqOption::_MqOption() {
    mClientRecvBuffSize = DefaultClientRecvBuffSize;
    mAckTimeout = DefaultAckTimeout;
    mReDeliveryInterval = DefaultReDeliveryInterval;
    mReDeliveryTimes = DefaultReDeliveryTimes;
}

_MqOption *_MqOption::setClientRecvBuffSize(int size) {
    mClientRecvBuffSize = size;
    return this;
}

_MqOption *_MqOption::setAckTimeout(int value) {
    mAckTimeout = value;
    return this;
}

_MqOption *_MqOption::setReDeliveryInterval(long value) {
    mReDeliveryInterval = value;
    return this;
}

_MqOption *_MqOption::setReDeliveryTimes(int value) {
    mReDeliveryTimes = value;
    return this;
}

int _MqOption::getClientRecvBuffSize() {
    return mClientRecvBuffSize;
}

int _MqOption::getAckTimeout() {
    return mAckTimeout;
}

long _MqOption::getReDeliveryInterval() {
    return mReDeliveryInterval;
}

int _MqOption::getReDeliveryTimes() {
    return mReDeliveryTimes;
}



}
