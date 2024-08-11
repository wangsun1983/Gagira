#include "FenceMessage.hpp"

using namespace obotcha;

namespace gagira {

_FenceMessage::_FenceMessage() {
    this->event = st(FenceMessage)::Type::Idle;
    this->privateData = 0;
}

_FenceMessage::_FenceMessage(st(FenceMessage)::Type event,String name,uint64_t waittime) {
    this->event = event;
    this->fencename = name;
    this->privateData = waittime;
}

uint64_t _FenceMessage::getResult() {
    return privateData;
}

uint64_t _FenceMessage::getWaitTime() {
    return privateData;
}

st(FenceMessage)::Type _FenceMessage::getEvent() {
    return static_cast<st(FenceMessage)::Type>(event);
}

String _FenceMessage::getFenceName() {
    return fencename;
}

_ConfirmFenceMessage::_ConfirmFenceMessage() {
    this->event = st(FenceMessage)::Type::ConfirmFence;
    this->privateData = 0;
}

_ConfirmFenceMessage::_ConfirmFenceMessage(String name,uint64_t result) {
    this->event = st(FenceMessage)::Type::ConfirmFence;
    this->fencename = name;
    this->privateData = result;
}

}
