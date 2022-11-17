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

    int getClientRecvBuffSize();

private:
    static const int DefaultClientRecvBuffSize;
    static const int DefaultThreadNum;
    static const int DefaultBufferSize;
    static const int DefaultAckTimeout;
    static const int DefaultRedeliveryInterval;
    static const int DefaultRedeliveryTimes;

    int mClientRecvBuffSize;
};

}

#endif
