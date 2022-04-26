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
    _MqCenterBuilder * setPersistentComponent(MqPersistentComponent);
    _MqCenterBuilder * setAckTimeout(int);
    
    MqCenter build();

private:
    static const int DefaultThreadNum;
    static const int DefaultBufferSize;
    static const int DefaultAckTimeout;
    
    String mUrl;
    int mThreadNum;
    int mBuffSize;
    int mAckTimeout;
    MqPersistentComponent mPersistentComp;
};

}

#endif
