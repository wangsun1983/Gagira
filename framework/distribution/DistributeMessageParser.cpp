#include "DistributeMessageParser.hpp"

namespace gagira {

_DistributeMessageParser::_DistributeMessageParser(int buffsize) {
    mBuffer = createByteRingArray(buffsize);
    mReader = createByteRingArrayReader(mBuffer);
    mCurrentMsgLen = 0;
}

void _DistributeMessageParser::pushData(ByteArray data) {
    printf("mBuffer capacity is %d,data size is %d \n",mBuffer->getCapacity(),data->size());
    mBuffer->push(data);
}

int _DistributeMessageParser::getBufferSize() {
    return mBuffer->getCapacity();
}

ArrayList<ByteArray> _DistributeMessageParser::doParse() {
    ArrayList<ByteArray> result = createArrayList<ByteArray>();

    while(1) {
        int availableDataSize = mBuffer->getStoredDataSize();
        printf("DistributeMessageParser availableDataSize is %d,mCurrentMsgLen is %d \n",availableDataSize,mCurrentMsgLen);
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
    printf("DistributeMessageParser,result size is %d \n",result->size());
    return result;
}


}
    
