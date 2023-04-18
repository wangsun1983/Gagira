#ifndef __GAGRIA_DISTRIBUTE_MESSAGE_PARSER_HPP__
#define __GAGRIA_DISTRIBUTE_MESSAGE_PARSER_HPP__

#include "Object.hpp"
#include "ByteArray.hpp"
#include "ByteRingArray.hpp"
#include "ByteRingArrayReader.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DistributeMessageParser) {
public:
    _DistributeMessageParser(int buffsize);

    void pushData(ByteArray);
    ArrayList<ByteArray> doParse();

    int getBufferSize();

private:
    ByteRingArray mBuffer;
    ByteRingArrayReader mReader;
    uint32_t mCurrentMsgLen;
};

}

#endif
