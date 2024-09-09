#include "QueueMessage.hpp"
#include "ByteArrayWriter.hpp"
#include "StringBuffer.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

//------ QueueMessage -----
uint32_t _QueueMessage::getEvent() {
    return event;
}

void _QueueMessage::setEvent(uint32_t event) {
    this->event = event;
}

void _QueueMessage::setData(ByteArray data) {
    this->data = data;
}

ByteArray _QueueMessage::getData() {
    return data;
}

uint32_t _QueueMessage::getReqId() {
    return reqId;
}

void _QueueMessage::setReqId(uint32_t reqid) {
    this->reqId = reqid;
}

uint32_t _QueueMessage::getResult() {
    return result;
}

void _QueueMessage::setResult(uint32_t result) {
    this->result = result;
}

 void _QueueMessage::setCancelId(uint32_t cancelid) {
    this->cancelId = cancelid;
 }

uint32_t _QueueMessage::getCancelId() {
    return cancelId;
}

_QueueMessage::_QueueMessage() {
    this->event = EventType::None;
    this->reqId = 0;
    this->data = nullptr;
}

_QueueMessage::_QueueMessage(uint32_t event,uint32_t reqid,ByteArray data) {
    this->event = event;
    this->reqId = reqid;
    this->data = data;
}

//----- CancelWaitMessage -----
_CancelWaitMessage::_CancelWaitMessage(uint32_t reqid,uint32_t cancelid) {
    this->event = EventType::Cancel;
    this->cancelId = cancelid;
    this->reqId = reqid;
}

//----- ReceiveMessageResponse -----
_MessageResponse::_MessageResponse() {
    this->event = EventType::AckFromServer;
    this->reqId = 0;
    this->result = 0;
    this->data = nullptr;
}

_MessageResponse::_MessageResponse(uint32_t reqid,uint32_t result,ByteArray data) {
    this->event = EventType::AckFromServer;
    this->reqId = reqid;
    this->result = result;
    this->data = data;
}

//----- MessageClientAck -----
_MessageClientAck::_MessageClientAck(uint32_t reqid) {
    this->event = EventType::AckFromClient;
    this->reqId = reqid;
}

}

