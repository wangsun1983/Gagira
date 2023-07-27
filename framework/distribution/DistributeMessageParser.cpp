#include "DistributeMessageParser.hpp"

namespace gagira {

_DistributeMessageParser::_DistributeMessageParser(int buffsize) {
    mBuffer = createByteRingArray(buffsize);
    mReader = createByteRingArrayReader(mBuffer);
    mCurrentMsgLen = 0;
}

void _DistributeMessageParser::pushData(ByteArray data) {
    mBuffer->push(data);
}

int _DistributeMessageParser::getBufferSize() {
    return mBuffer->getCapacity();
}

ArrayList<ByteArray> _DistributeMessageParser::doParse() {
    ArrayList<ByteArray> result = createArrayList<ByteArray>();

    while(1) {
        int availableDataSize = mBuffer->getStoredDataSize();
        if(mCurrentMsgLen != 0) {
            if(mCurrentMsgLen <= availableDataSize) {
                mReader->move(mCurrentMsgLen);
                ByteArray data = mReader->pop();
                result->add(data);
                mCurrentMsgLen = 0;
                continue;
            }
        } else if(mReader->read<uint32_t>(mCurrentMsgLen)
                    == ContinueRead) {
            continue;
        }
        break;
    }
    return result;
}


}
    
