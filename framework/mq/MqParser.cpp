#include "MqParser.hpp"

namespace gagira {

_MqParser::_MqParser(int buffsize) {
    mBuffer = createByteRingArray(buffsize);
    mReader = createByteRingArrayReader(mBuffer);
    mCurrentMsgLen = 0;
}

void _MqParser::pushData(ByteArray data) {
    mBuffer->push(data);
}

ArrayList<ByteArray> _MqParser::doParse() {
    ArrayList<ByteArray> result = createArrayList<ByteArray>();

    while(1) {
        int availableDataSize = mBuffer->getAvailDataSize();
        if(mCurrentMsgLen != 0) {
            if(mCurrentMsgLen <= availableDataSize) {
                mReader->move(mCurrentMsgLen);
                ByteArray data = mReader->pop();
                result->add(data);
                mCurrentMsgLen = 0;
                continue;
            }
        } else if(mReader->read<uint32_t>(mCurrentMsgLen)
                    == st(ByteRingArrayReader)::Continue) {
            continue;
        }
        break;
    }

    return result;
}


}
    
