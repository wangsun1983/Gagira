#include "SpaceCenter.hpp"
#include "SocketBuilder.hpp"
#include "HttpUrl.hpp"
#include "Inspect.hpp"

namespace gagira {

_SpaceCenter::_SpaceCenter(String url,DistributeOption option):_DistributeCenter(url,option) {

}

_SpaceCenter::~_SpaceCenter() {

}

int _SpaceCenter::onMessage(DistributeLinker linker,ByteArray data) {
    
}

int _SpaceCenter::onNewClient(DistributeLinker linker) {
    return 0;
}

int _SpaceCenter::onDisconnectClient(DistributeLinker linker) {
    return 0;
}

}
