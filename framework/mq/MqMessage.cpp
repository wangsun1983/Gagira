#include <mutex>

#include "MqMessage.hpp"
#include "ByteArrayWriter.hpp"
#include "System.hpp"

namespace gagira {

_MqMessage::_MqMessage() {
    mRetryTimes = 0;
    mFlags = 0;
}

_MqMessage::_MqMessage(String channel,ByteArray data,uint32_t flags):_MqMessage() {
    mData = data;
    mChannel = channel;
    mFlags = flags;
}

_MqMessage::_MqMessage(String channel,String acktoken,ByteArray data,uint32_t types):
                      _MqMessage(channel,data,types) {
    mAckToken = acktoken;
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

ByteArray _MqMessage::getData() {
    return mData;
}

void _MqMessage::clearData() {
    mData = nullptr;
}

String _MqMessage::getChannel() {
    return mChannel;
}

void _MqMessage::setFlags(uint32_t flag) {
    mFlags = flag;
}


String _MqMessage::getAckToken() {
    return mAckToken;
}

void _MqMessage::setAckToken(String token) {
    mAckToken = token;
}

String _MqMessage::getStickToken() {
    return mStickToken;
}

void _MqMessage::setStickToken(String token) {
    mStickToken = token;
}

int _MqMessage::getRetryTimes() {
    return mRetryTimes;
}

void _MqMessage::setRetryTimes(int times) {
    mRetryTimes = times;
}

uint32_t _MqMessage::getType() {
    return mFlags & ~(-MaxMessageType);
}

bool _MqMessage::isOneShot() {
    return (mFlags & OneShotFlag) != 0;
}

bool _MqMessage::isAcknowledge() {
    return (mFlags & AcknowledgeFlag) != 0;
}

bool _MqMessage::isStick() {
    return (mFlags & StickFlag) != 0;
}

bool _MqMessage::isUnStick() {
    return (mFlags & UnStickFlag) != 0;
}

bool _MqMessage::isPersist() {
    return (mFlags & PersistFlag) != 0;
}

bool _MqMessage::isStart() {
    return (mFlags & StartFalg) != 0;
}

bool _MqMessage::isComplete() {
    return (mFlags & CompleteFlag) != 0;
}

void _MqMessage::setTTL(long value) {
    mTTL = value;
    mExpireTime = st(System)::currentTimeMillis();
}

long _MqMessage::getTTL() {
    return mTTL;
}

long _MqMessage::getExpireTime() {
    return mExpireTime;
}

}
