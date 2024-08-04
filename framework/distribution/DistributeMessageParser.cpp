#include "DistributeMessageParser.hpp"

namespace gagira {

_DistributeMessageParser::_DistributeMessageParser(int buffsize) {
    mBuffer = ByteRingArray::New(buffsize);
    mReader = ByteRingArrayReader::New(mBuffer);
    mCurrentMsgLen = 0;
}

void _DistributeMessageParser::pushData(ByteArray data) {
    mBuffer->push(data);
}

int _DistributeMessageParser::getBufferSize() {
    return mBuffer->getCapacity();
}

ArrayList<ByteArray> _DistributeMessageParser::doParse() {
    ArrayList<ByteArray> result = ArrayList<ByteArray>::New();

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
                    == st(IO)::Reader::Result::HasContent) {
            continue;
        }
        break;
    }
    return result;
}


}
    
