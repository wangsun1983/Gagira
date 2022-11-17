#include "MqOption.hpp"

using namespace obotcha;

namespace gagira {

const int _MqOption::DefaultClientRecvBuffSize = 4096*4;
const int _MqOption::DefaultThreadNum = 4;
const int _MqOption::DefaultBufferSize = 1024*4;
const int _MqOption::DefaultAckTimeout = 1000;
const int _MqOption::DefaultRedeliveryInterval = 1000;
const int _MqOption::DefaultRedeliveryTimes = 3;

_MqOption::_MqOption() {
    mClientRecvBuffSize = DefaultClientRecvBuffSize;
}

_MqOption *_MqOption::setClientRecvBuffSize(int size) {
    mClientRecvBuffSize = size;
    return this;
}


int _MqOption::getClientRecvBuffSize() {
    return mClientRecvBuffSize;
}



}
