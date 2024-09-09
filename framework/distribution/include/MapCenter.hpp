#ifndef __GAGRIA_MAP_CENTOR_HPP__
#define __GAGRIA_MAP_CENTOR_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "SocketMonitor.hpp"
#include "MapOption.hpp"
#include "InetAddress.hpp"
#include "ServerSocket.hpp"
#include "DistributeCenter.hpp"
#include "DistributeOption.hpp"
#include "DistributeMessageConverter.hpp"
#include "DistributeHandler.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MapCenter) IMPLEMENTS(DistributeCenter) {
public:
    _MapCenter(String url,DistributeOption);
    ~_MapCenter();
    int onMessage(DistributeLinker,ByteArray);
    int onNewClient(DistributeLinker);
    int onDisconnectClient(DistributeLinker);

private:
    DistributeMessageConverter mConverter;

    ConcurrentHashMap<String,ByteArray> mDatas;
    ConcurrentHashMap<String,ArrayList<DistributeLinker>> mListeners;
    DistributeHandler mHandler;

    void response(DistributeLinker linker,uint32_t response,uint32_t event,int result,String tag = nullptr,ByteArray data = nullptr);
};

}

#endif
