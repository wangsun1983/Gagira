#ifndef __GAGRIA_MQ_PARSER_HPP__
#define __GAGRIA_MQ_PARSER_HPP__

#include "Object.hpp"
#include "ByteArray.hpp"
#include "ByteRingArray.hpp"
#include "ByteRingArrayReader.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqParser) {
public:
    _MqParser(int buffsize);

    void pushData(ByteArray);
    ArrayList<ByteArray> doParse();

private:
    ByteRingArray mBuffer;
    ByteRingArrayReader mReader;
    uint32_t mCurrentMsgLen;
};

}

#endif
