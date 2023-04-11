#include "BroadcastDLQMessage.hpp"

using namespace obotcha;

namespace gagira {

_BroadcastDLQMessage::_BroadcastDLQMessage() {
}

int _BroadcastDLQMessage::getCode() {
    return mCode;
}

ByteArray _BroadcastDLQMessage::getData() {
    return mMessageData;
}

String _BroadcastDLQMessage::getSrcAddress() {
    return mSrcAddress;
}

String _BroadcastDLQMessage::getDestAddress() {
    return mDestAddress;
}

String _BroadcastDLQMessage::getToken() {
    return mToken;
}

long _BroadcastDLQMessage::getPointTime() {
    return mPointTime;
}

_BroadcastDLQMessage *_BroadcastDLQMessage::setCode(int code) {
    mCode = code;
    return this;
}

_BroadcastDLQMessage *_BroadcastDLQMessage::setData(ByteArray data) {
    mMessageData = data;
    return this;
}

_BroadcastDLQMessage *_BroadcastDLQMessage::setToken(String token) {
    mToken = token;
    return this;
}

_BroadcastDLQMessage *_BroadcastDLQMessage::setSrcAddress(String addr) {
    mSrcAddress = addr;
    return this;
}

_BroadcastDLQMessage *_BroadcastDLQMessage::setDestAddress(String addr) {
    mDestAddress = addr;
    return this;
}

_BroadcastDLQMessage *_BroadcastDLQMessage::setPointTime(long time) {
    mPointTime = time;
    return this;
} 

}
