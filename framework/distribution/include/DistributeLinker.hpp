#ifndef __GAGRIA_DISTRIBUTE_LINKER_HPP__
#define __GAGRIA_DISTRIBUTE_LINKER_HPP__

#include "Object.hpp"
#include "DistributeMessageParser.hpp"
#include "Socket.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DistributeLinker) {
public:
    _DistributeLinker(Socket,int buffsize = 1024*32);
    ArrayList<ByteArray> doParse(ByteArray);
    Socket getSocket();
    
private:
    Socket mSocket;
    DistributeMessageParser mParser;
    int mBuffSize;
};

}

#endif
