#include "MapMessage.hpp"
#include "ByteArrayWriter.hpp"
#include "StringBuffer.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

_MapMessage::_MapMessage() {
    //do nothing
}

_MapMessage::_MapMessage(uint64_t id,int event,String tag,ByteArray data,String md5sum) {
    this->id = id;
    this->event = event;
    this->tag = tag;
    this->data = data;
    this->md5sum = md5sum;
}

uint64_t _MapMessage::getId() {
    return id;
}

bool _MapMessage::isNeedResponse() {
    return (id > 0);
}

uint32_t _MapMessage::getEvent() {
    return event;
}

String _MapMessage::getTag() {
    return tag;
}

ByteArray _MapMessage::getData() {
    return data;
}

uint32_t _MapMessage::getResult() {
    return result;
}

//----SapceMonitorMessage----
_MapMonitorMessage::_MapMonitorMessage(uint64_t id,ArrayList<String> list) {
    this->event = Monitor;
    this->id = id;
    StringBuffer buffer = StringBuffer::New();
    ForEveryOne(l,list) {
        buffer->append(l);
        buffer->append(",");
    }
    this->data = buffer->subString(0,buffer->size() - 1)->toString()->toByteArray();
}

_MapMonitorMessage::_MapMonitorMessage(uint64_t id,String tag) {
    this->event = Monitor;
    this->tag = tag;
    this->id = id;
}

//----SapceMonitorMessage----
_MapUnMonitorMessage::_MapUnMonitorMessage(uint64_t id,ArrayList<String> list) {
    this->id = id;
    this->event = UnMonitor;
    StringBuffer buffer = StringBuffer::New();
    ForEveryOne(l,list) {
        buffer->append(l);
        buffer->append(",");
    }
    this->data = buffer->subString(0,buffer->size() - 1)->toString()->toByteArray();
}

_MapUnMonitorMessage::_MapUnMonitorMessage(uint64_t id,String tag) {
    this->id = id;
    this->event = UnMonitor;
    this->tag = tag;
}

//---SapceUpdateMessage---
_MapUpdateMessage::_MapUpdateMessage(uint64_t id,String tag,ByteArray value,String md5sum) {
    this->id = id;
    this->event = Update;
    this->tag = tag;
    this->data = value;
    this->md5sum = md5sum;
}

//---SapceAcquireMessage---
_MapAcquireMessage::_MapAcquireMessage(uint64_t id,String tag) {
    this->id = id;
    this->event = Acquire;
    this->tag = tag;
}

_MapRemoveMessage::_MapRemoveMessage(uint64_t id,String tag,String md5sum) {
    this->id = id;
    this->event = Remove;
    this->tag = tag;
    this->md5sum = md5sum;
}

//----- MapMessageResponse ------
_MapMessageResponse::_MapMessageResponse(uint64_t id,uint32_t event,int result,String tag,ByteArray data) {
    this->id = id;
    this->result = result;
    this->tag = tag;
    this->data = data;
    this->event = event;
}

_MapMessageResponse::_MapMessageResponse(uint64_t id,uint32_t event,int result) {
    this->id = id;
    this->result = result;
    this->event = event;
}

}

