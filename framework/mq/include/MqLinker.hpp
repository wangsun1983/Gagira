#ifndef __GAGRIA_MQ_LINKER_HPP__
#define __GAGRIA_MQ_LINKER_HPP__

#include "Object.hpp"
#include "MqParser.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqLinker) {
public:
    _MqLinker(int buffsize);

    MqParser getParser();

private:
    MqParser mParser;
    int mBuffSize;
};

}

#endif
