#include <mutex>

#include "BroadcastMessage.hpp"
#include "ByteArrayWriter.hpp"
#include "System.hpp"

namespace gagira {

UUID _BroadcastMessage::mUuid = createUUID();
Sha _BroadcastMessage::mSha = createSha(SHA_256);

//-------- BroadcastMessage 
_BroadcastMessage::_BroadcastMessage() {
    mRetryTimes = 0;
    mFlags = 0;
    mExpireTime = 0;
    mTTL = 0;
    mPublishTime = 0;
    mToken = mSha->encrypt(mUuid->generate()->append(
                            createString(st(System)::currentTimeMillis())));
}

_BroadcastMessage::_BroadcastMessage(String channel,ByteArray data,uint32_t flags):_BroadcastMessage() {
    mData = data;
    mChannel = channel;
    mFlags = flags;
}

// ByteArray _BroadcastMessage::generatePacket() {
//     ByteArray serializeData = serialize();
//     ByteArray finalData = createByteArray(serializeData->size() + sizeof(uint32_t));
//     ByteArrayWriter writer = createByteArrayWriter(finalData);
//     writer->write<uint32_t>(serializeData->size());
//     writer->write(serializeData);

//     return finalData;
// }

// BroadcastMessage _BroadcastMessage::generateMessage(ByteArray data) {
//     ByteArray msgData = createByteArray(data->toValue() + sizeof(uint32_t),data->size() - sizeof(uint32_t),true);
//     BroadcastMessage msg = createBroadcastMessage();
//     msg->deserialize(msgData);
//     return msg;
// }

ByteArray _BroadcastMessage::getData() {
    return mData;
}

void _BroadcastMessage::clearData() {
    mData = nullptr;
}

String _BroadcastMessage::getChannel() {
    return mChannel;
}

void _BroadcastMessage::setFlags(uint32_t flag) {
    mFlags = flag;
}


String _BroadcastMessage::getToken() {
    return mToken;
}

int _BroadcastMessage::getRetryTimes() {
    return mRetryTimes;
}

void _BroadcastMessage::setRetryTimes(int times) {
    mRetryTimes = times;
}

void _BroadcastMessage::setData(ByteArray data) {
    mData = data;
}

void _BroadcastMessage::setChannel(String channel) {
    mChannel = channel;
}

void _BroadcastMessage::setToken(String token) {
    mToken = token;
}

uint32_t _BroadcastMessage::getType() {
    return mFlags & ~(-MaxMessageType);
}

bool _BroadcastMessage::isOneShot() {
    return (mFlags & OneShotFlag) != 0;
}

bool _BroadcastMessage::isAcknowledge() {
    return (mFlags & AcknowledgeFlag) != 0;
}

bool _BroadcastMessage::isStick() {
    return (mFlags & StickFlag) != 0;
}

bool _BroadcastMessage::isUnStick() {
    return (mFlags & UnStickFlag) != 0;
}

bool _BroadcastMessage::isPersist() {
    return (mFlags & PersistFlag) != 0;
}

bool _BroadcastMessage::isStart() {
    return (mFlags & StartFalg) != 0;
}

bool _BroadcastMessage::isComplete() {
    return (mFlags & CompleteFlag) != 0;
}

void _BroadcastMessage::setTTL(long value) {
    if(value != 0) {
        mTTL = value;
        mExpireTime = st(System)::currentTimeMillis();
    }
}

void _BroadcastMessage::setPublishTime(long time) {
    mPublishTime = time;
}

long _BroadcastMessage::getTTL() {
    return mTTL;
}

long _BroadcastMessage::getExpireTime() {
    return mExpireTime;
}

long _BroadcastMessage::getPublishTime() {
    return mPublishTime;
}

}
