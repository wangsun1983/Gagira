#ifndef __GAGRIA_MQ_CENTOR_BUILDER_HPP__
#define __GAGRIA_MQ_CENTOR_BUILDER_HPP__

#include "String.hpp"
#include "MqPersistentComponent.hpp"
#include "MqCenter.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqCenterBuilder) {
public:
    _MqCenterBuilder();

    _MqCenterBuilder * setUrl(String url);
    _MqCenterBuilder * setThreadNum(int num);
    _MqCenterBuilder * setBufferSize(int);
    _MqCenterBuilder * setPersistence(MqPersistenceInterface);
    _MqCenterBuilder * setAckTimeout(int);
    _MqCenterBuilder * setRedeliveryInterval(int);
    _MqCenterBuilder * setRedeliveryTimes(int);
    
    MqCenter build();

private:
    static const int DefaultThreadNum;
    static const int DefaultBufferSize;
    static const int DefaultAckTimeout;
    static const int DefaultRedeliveryInterval;
    static const int DefaultRedeliveryTimes;

    String mUrl;
    int mThreadNum;
    int mBuffSize;
    int mAckTimeout;
    int mRedeliveryInterval;
    int mRedeliveryTimes;
    
    MqPersistenceInterface mPersistence;
};

}

#endif
