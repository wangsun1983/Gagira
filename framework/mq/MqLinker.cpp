#include "MqLinker.hpp"

namespace gagira {

_MqLinker::_MqLinker(int buffsize) {
    mBuffSize = buffsize;
    mParser = createMqParser(buffsize);
}

MqParser _MqLinker::getParser() {
    return mParser;
}

}
