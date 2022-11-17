#ifndef __GAGRIA_MQ_CENTOR_BUILDER_HPP__
#define __GAGRIA_MQ_CENTOR_BUILDER_HPP__

#include "String.hpp"
#include "MqOption.hpp"
#include "MqCenter.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqCenterBuilder) {
public:
    _MqCenterBuilder();

    _MqCenterBuilder * setUrl(String url);
    _MqCenterBuilder * setOption(MqOption option);
    
    MqCenter build();

private:
    String mUrl;
    MqOption mOption;
};

}

#endif
