#ifndef __GAGRIA_DISTRIBUTE_CENTOR_BUILDER_HPP__
#define __GAGRIA_DISTRIBUTE_CENTOR_BUILDER_HPP__

#include "String.hpp"
#include "DistributeOption.hpp"
#include "MqCenter.hpp"
#include "SpaceCenter.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DistributeCenterBuilder) {
public:
    _DistributeCenterBuilder();

    _DistributeCenterBuilder * setUrl(String url);
    _DistributeCenterBuilder * setOption(DistributeOption option);
    
    SpaceCenter buildSpaceCenter();
    MqCenter buildMqCenter();

private:
    String mUrl;
    DistributeOption mOption;
};

}

#endif
