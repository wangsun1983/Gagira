#ifndef __GAGRIA_DISTRIBUTE_CENTOR_BUILDER_HPP__
#define __GAGRIA_DISTRIBUTE_CENTOR_BUILDER_HPP__

#include "String.hpp"
#include "DistributeOption.hpp"
#include "BroadcastCenter.hpp"
#include "MapCenter.hpp"
#include "QueueCenter.hpp"
#include "ArchiveCenter.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DistributeCenterBuilder) {
public:
    _DistributeCenterBuilder();

    _DistributeCenterBuilder * setUrl(String url);
    _DistributeCenterBuilder * setOption(DistributeOption option);
    
    MapCenter buildSpaceCenter();
    BroadcastCenter buildBroadcastCenter();
    QueueCenter buildQueueCenter();
    ArchiveCenter buildArchiveCenter();

private:
    String mUrl;
    DistributeOption mOption;
};

}

#endif
