#include <mutex>

#include "MqMessage.hpp"
#include "ByteArrayWriter.hpp"

namespace gagira {

_MqMessage::_MqMessage() {
    retryTimes = 0;
}

_MqMessage::_MqMessage(String channel,ByteArray data,uint32_t flags):_MqMessage() {
    this->data = data;
    this->channel = channel;
    this->flags = flags;
}

bool _MqMessage::isPersist() {
    return (flags&Persistent) != 0;
}

bool _MqMessage::isAcknowledge() {
    printf("flags is %lx,result is %lx,ack is %lx,oneshot is %lx \n",flags,flags&Acknowledge,Acknowledge,PublishOneShot);

    return (flags&Acknowledge) != 0;
}

ByteArray _MqMessage::toByteArray() {
    ByteArray serializeData = serialize();
    ByteArray finalData = createByteArray(serializeData->size() + 4);
    ByteArrayWriter writer = createByteArrayWriter(finalData);
    writer->writeInt(serializeData->size());
    writer->writeByteArray(serializeData);

    return finalData;
}

int _MqMessage::getType() {
    return (flags & ~MaxMessageType);
}

ByteArray _MqMessage::getData() {
    return data;
}

String _MqMessage::getChannel() {
    return channel;
}

void _MqMessage::setFlags(uint32_t flag) {
    flags = flag;
}

String _MqMessage::getToken() {
    return token;
}

void _MqMessage::setToken(String s) {
    token = s;
}

int _MqMessage::getRetryTimes() {
    return retryTimes;
}

void _MqMessage::setRetryTimes(int s) {
    retryTimes = s;
}

}