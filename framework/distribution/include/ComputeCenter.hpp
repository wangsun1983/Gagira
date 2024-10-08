#ifndef __GAGRIA_COMPUTE_CENTOR_HPP__
#ifndef __GAGRIA_COMPUTE_CENTOR_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "SocketMonitor.hpp"
#include "MapOption.hpp"
#include "InetAddress.hpp"
#include "ServerSocket.hpp"
#include "DistributeCenter.hpp"
#include "DistributeOption.hpp"
#include "DistributeMessageConverter.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ComputeCenter) IMPLEMENTS(DistributeCenter) {
public:
    _ComputeCenter(String url,DistributeOption);
    ~_SpaceCenter();
    int onMessage(DistributeLinker,ByteArray);
    int onNewClient(DistributeLinker);
    int onDisconnectClient(DistributeLinker);

private:
    DistributeMessageConverter mConverter;

    ConcurrentHashMap<String,ByteArray> mDatas;
    ConcurrentHashMap<String,ArrayList<DistributeLinker>> mListeners;
};

}

#endif
