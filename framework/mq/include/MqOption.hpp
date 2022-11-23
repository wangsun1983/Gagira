#ifndef __GAGRIA_MQ_OPTION_HPP__
#define __GAGRIA_MQ_OPTION_HPP__

#include "Object.hpp"
#include "SocketOption.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqOption) IMPLEMENTS(SocketOption) {
public:
    _MqOption();

    _MqOption *setClientRecvBuffSize(int);
    _MqOption *setAckTimeout(int);
    _MqOption *setReDeliveryInterval(long);
    _MqOption *setReDeliveryTimes(int);

    int getClientRecvBuffSize();
    int getAckTimeout();
    long getReDeliveryInterval();
    int getReDeliveryTimes();

private:
    static const int DefaultClientRecvBuffSize;
    static const int DefaultThreadNum;
    static const int DefaultBufferSize;
    static const long DefaultAckTimeout;
    static const int DefaultReDeliveryInterval;
    static const int DefaultReDeliveryTimes;

    int mClientRecvBuffSize;
    int mAckTimeout;
    long mReDeliveryInterval;
    int mReDeliveryTimes;
};

}

#endif
