#include "SpaceMessage.hpp"
#include "ByteArrayWriter.hpp"
#include "StringBuffer.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

_SpaceMessage::_SpaceMessage() {
    //do nothing
}

_SpaceMessage::_SpaceMessage(int event,String tag,ByteArray data,String md5sum) {
    this->event = event;
    this->tag = tag;
    this->data = data;
    this->md5sum = md5sum;
}

//----SapceMonitorMessage----
_SpaceMonitorMessage::_SpaceMonitorMessage(ArrayList<String> list) {
    this->event = Monitor;
    StringBuffer buffer = StringBuffer::New();
    ForEveryOne(l,list) {
        buffer->append(l);
        buffer->append(",");
    }
    this->data = buffer->subString(0,buffer->size() - 1)->toString()->toByteArray();
}

_SpaceMonitorMessage::_SpaceMonitorMessage(String tag) {
    this->event = Monitor;
    this->tag = tag;
}

//----SapceMonitorMessage----
_SpaceUnMonitorMessage::_SpaceUnMonitorMessage(ArrayList<String> list) {
    this->event = UnMonitor;
    StringBuffer buffer = StringBuffer::New();
    ForEveryOne(l,list) {
        buffer->append(l);
        buffer->append(",");
    }
    this->data = buffer->subString(0,buffer->size() - 1)->toString()->toByteArray();
}

_SpaceUnMonitorMessage::_SpaceUnMonitorMessage(String tag) {
    this->event = UnMonitor;
    this->tag = tag;
}

//---SapceUpdateMessage---
_SpaceUpdateMessage::_SpaceUpdateMessage(String tag,ByteArray value,String md5sum) {
    this->event = Update;
    this->tag = tag;
    this->data = value;
    this->md5sum = md5sum;
}

//---SapceAcquireMessage---
_SpaceAcquireMessage::_SpaceAcquireMessage(String tag) {
    this->event = Acquire;
    this->tag = tag;
}

//---SapceRemoveMessage---
_SpaceRemoveMessage::_SpaceRemoveMessage(String tag,String md5sum) {
    this->event = Remove;
    this->tag = tag;
    this->md5sum = md5sum;
}

//---SpaceAcquireMessageResult---
_SpaceAcquireMessageResult::_SpaceAcquireMessageResult(int result,ByteArray data) {
    this->result = result;
    this->data = data;
}

//---SpaceUpdateMessageResult---
_SpaceUpdateMessageResult::_SpaceUpdateMessageResult(int result,ByteArray data) {
    this->result = result;
    this->data = data;
}

//---SpaceNotifyMessage---
_SpaceNotifyMessage::_SpaceNotifyMessage(int event,String tag,ByteArray data) {
    this->event = event;
    this->tag = tag;
    this->data = data;
}

}

