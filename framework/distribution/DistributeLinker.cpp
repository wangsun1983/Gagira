#include "DistributeLinker.hpp"

namespace gagira {

_DistributeLinker::_DistributeLinker(Socket sock,int buffsize) {
    mSocket = sock;
    mBuffSize = buffsize;
    mParser = DistributeMessageParser::New(buffsize);
}

ArrayList<ByteArray> _DistributeLinker::doParse(ByteArray data) {
    mParser->pushData(data);
    return mParser->doParse();
}

Socket _DistributeLinker::getSocket() {
    return mSocket;
}


}
