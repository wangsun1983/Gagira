#include "MqDLQMessage.hpp"

using namespace obotcha;

namespace gagira {

_MqDLQMessage::_MqDLQMessage() {
}

int _MqDLQMessage::getCode() {
    return mCode;
}

ByteArray _MqDLQMessage::getData() {
    return mMessageData;
}

String _MqDLQMessage::getSrcAddress() {
    return mSrcAddress;
}

String _MqDLQMessage::getDestAddress() {
    return mDestAddress;
}

String _MqDLQMessage::getToken() {
    return mToken;
}

long _MqDLQMessage::getPointTime() {
    return mPointTime;
}

_MqDLQMessage *_MqDLQMessage::setCode(int code) {
    mCode = code;
    return this;
}

_MqDLQMessage *_MqDLQMessage::setData(ByteArray data) {
    mMessageData = data;
    return this;
}

_MqDLQMessage *_MqDLQMessage::setToken(String token) {
    mToken = token;
    return this;
}

_MqDLQMessage *_MqDLQMessage::setSrcAddress(String addr) {
    mSrcAddress = addr;
    return this;
}

_MqDLQMessage *_MqDLQMessage::setDestAddress(String addr) {
    mDestAddress = addr;
    return this;
}

_MqDLQMessage *_MqDLQMessage::setPointTime(long time) {
    mPointTime = time;
    return this;
} 

}
