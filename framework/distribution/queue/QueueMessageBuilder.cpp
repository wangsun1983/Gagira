#include "QueueMessageBuilder.hpp"
#include "AutoLock.hpp"

namespace gagira {

_QueueMessageBuilder::_QueueMessageBuilder() {
    mMutex = Mutex::New();
    data = nullptr;
    reqIdCounter = 0;
}

_QueueMessageBuilder *_QueueMessageBuilder::setData(ByteArray data) {
    this->data = data;
    return this;
}

_QueueMessageBuilder *_QueueMessageBuilder::setCancelId(uint32_t reqid) {
    this->cancelId = reqid;
    return this;
}

_QueueMessageBuilder *_QueueMessageBuilder::setAckReqId(uint32_t reqId) {
    this->ackReqId = reqId;
    return this;
}

_QueueMessageBuilder * _QueueMessageBuilder::setAsRequire() {
    event = st(QueueMessage)::Acquire;
    return this;
}
_QueueMessageBuilder * _QueueMessageBuilder::setAsSubmit() {
    event = st(QueueMessage)::Submit;
    return this;
}

_QueueMessageBuilder * _QueueMessageBuilder::setAsCancel() {
    event = st(QueueMessage)::Cancel;
    return this;
}

_QueueMessageBuilder * _QueueMessageBuilder::setAsAck() {
    event = st(QueueMessage)::AckFromClient;
    return this;
}

uint32_t _QueueMessageBuilder::getNextReqId() {
    AutoLock l(mMutex);
    reqIdCounter++;
    return reqIdCounter;
}

QueueMessage _QueueMessageBuilder::build() {
    QueueMessage msg = nullptr;
    if(event == st(QueueMessage)::AckFromClient) {
        msg = MessageClientAck::New(ackReqId);
    } else if(event == st(QueueMessage)::Cancel) {
        msg =  CancelWaitMessage::New(getNextReqId(),cancelId);
    } else {
        msg = QueueMessage::New(event,getNextReqId(),data);
    }
    
    cancelId = 0;
    ackReqId = 0;
    data = nullptr;
    event = st(QueueMessage)::None;
    return msg;
}

}