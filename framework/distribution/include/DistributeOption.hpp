#ifndef __GAGRIA_DISTRIBUTE_OPTION_HPP__
#define __GAGRIA_DISTRIBUTE_OPTION_HPP__

#include "Object.hpp"
#include "SocketOption.hpp"
#include "DistributeHandler.hpp"
#include "String.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DistributeOption) IMPLEMENTS(SocketOption) {
public:
    _DistributeOption();

    _DistributeOption *setClientRecvBuffSize(int);
    _DistributeOption *setAckTimeout(int);
    _DistributeOption *setReDeliveryInterval(long);
    _DistributeOption *setReDeliveryTimes(int);
    _DistributeOption *setWaitPostBack(bool);
    _DistributeOption *setHandler(DistributeHandler);

    int getClientRecvBuffSize();
    int getAckTimeout();
    long getReDeliveryInterval();
    int getReDeliveryTimes();
    bool getWaitPostBack();
    DistributeHandler getHandler();

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
    bool mWaitPostBack;
    DistributeHandler mHandler;
};

}

#endif
