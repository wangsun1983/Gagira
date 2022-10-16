#include <mutex>

#include "MqMessage.hpp"
#include "ByteArrayWriter.hpp"

namespace gagira {

_MqMessage::_MqMessage() {
    retryTimes = 0;
    token = nullptr;
    data = nullptr;
    flags = 0;
    channel = nullptr;
}

_MqMessage::_MqMessage(String channel,ByteArray data,uint32_t flags):_MqMessage() {
    this->data = data;
    this->channel = channel;
    this->flags = flags;
}

bool _MqMessage::isPersist() {
    return (flags&Persistent) != 0;
}

ByteArray _MqMessage::generatePacket() {
    ByteArray serializeData = serialize();
    ByteArray finalData = createByteArray(serializeData->size() + sizeof(uint32_t));
    ByteArrayWriter writer = createByteArrayWriter(finalData);
    writer->write<uint32_t>(serializeData->size());
    writer->write(serializeData);

    return finalData;
}

MqMessage _MqMessage::generateMessage(ByteArray data) {
    ByteArray msgData = createByteArray(data->toValue() + sizeof(uint32_t),data->size() - sizeof(uint32_t),true);
    MqMessage msg = createMqMessage();
    msg->deserialize(msgData);
    //msg->mPacketData = data;
    return msg;
}

bool _MqMessage::isAcknowledge() {
    return (flags&Acknowledge) != 0;
}

bool _MqMessage::isShakeHand() {
    return (flags & ShakeHand) != 0;
}

bool _MqMessage::isDetach() {
    return (flags & Detach) != 0;
}

bool _MqMessage::isSubscribe() {
    return (flags & Subscribe) != 0;
}

bool _MqMessage::isUnSubscribe() {
    return (flags & UnSubscribe) != 0;
}

bool _MqMessage::isPublish() {
    return (flags & Publish) != 0;
}

bool _MqMessage::isPublishOneShot() {
    return (flags & PublishOneShot) != 0;
}

bool _MqMessage::isAck() {
    return (flags & MessageAck) != 0;
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

uint32_t _MqMessage::getFlags() {
    return flags;
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
