#ifndef __GAGRIA_QUEUE_MESSAGE_BUILDER_HPP__
#define __GAGRIA_QUEUE_MESSAGE_BUILDER_HPP__

#include "Object.hpp"
#include "QueueMessage.hpp"
#include "Mutex.hpp"
#include "Uint32.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(QueueMessageBuilder) {
public:
    _QueueMessageBuilder();
    _QueueMessageBuilder *setData(ByteArray);
    _QueueMessageBuilder *setCancelId(uint32_t dataid);
    _QueueMessageBuilder *setAckReqId(uint32_t reqId);
    _QueueMessageBuilder * setAsRequire();
    _QueueMessageBuilder * setAsSubmit();
    _QueueMessageBuilder * setAsCancel();
    _QueueMessageBuilder * setAsAck();
    
    QueueMessage build();

private:
    uint32_t getNextReqId();

    Mutex mMutex;
    ByteArray data;
    uint32_t reqIdCounter;
    uint32_t event;
    uint32_t cancelId;
    uint32_t ackReqId;
};

}

#endif