#include "DistributeOption.hpp"

using namespace obotcha;

namespace gagira {

const int _DistributeOption::DefaultClientRecvBuffSize = 4096*4;
const int _DistributeOption::DefaultThreadNum = 4;
const int _DistributeOption::DefaultBufferSize = 1024*4;
const long _DistributeOption::DefaultAckTimeout = 1000;
const int _DistributeOption::DefaultReDeliveryInterval = 1000;
const int _DistributeOption::DefaultReDeliveryTimes = 3;

_DistributeOption::_DistributeOption() {
    mClientRecvBuffSize = DefaultClientRecvBuffSize;
    mAckTimeout = DefaultAckTimeout;
    mReDeliveryInterval = DefaultReDeliveryInterval;
    mReDeliveryTimes = DefaultReDeliveryTimes;
    mWaitPostBack = false;
}

_DistributeOption *_DistributeOption::setClientRecvBuffSize(int size) {
    mClientRecvBuffSize = size;
    return this;
}

_DistributeOption *_DistributeOption::setAckTimeout(int value) {
    mAckTimeout = value;
    return this;
}

_DistributeOption *_DistributeOption::setReDeliveryInterval(long value) {
    mReDeliveryInterval = value;
    return this;
}

_DistributeOption *_DistributeOption::setReDeliveryTimes(int value) {
    mReDeliveryTimes = value;
    return this;
}

_DistributeOption *_DistributeOption::setWaitPostBack(bool value) {
    mWaitPostBack = value;
    return this;
}

int _DistributeOption::getClientRecvBuffSize() {
    return mClientRecvBuffSize;
}

int _DistributeOption::getAckTimeout() {
    return mAckTimeout;
}

long _DistributeOption::getReDeliveryInterval() {
    return mReDeliveryInterval;
}

int _DistributeOption::getReDeliveryTimes() {
    return mReDeliveryTimes;
}

bool _DistributeOption::getWaitPostBack() {
    return mWaitPostBack;
}

}
