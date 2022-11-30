#include "MqLinker.hpp"

namespace gagira {

_MqLinker::_MqLinker(int buffsize) {
    mBuffSize = buffsize;
    mParser = createMqParser(buffsize);
}

ArrayList<ByteArray> _MqLinker::doParse(ByteArray data) {
    mParser->pushData(data);
    return mParser->doParse();
}

}
