#include "FenceMessage.hpp"

using namespace obotcha;

namespace gagira {

_FenceMessage::_FenceMessage() {
    this->event = 0;
    this->privateData = 0;
}

_FenceMessage::_FenceMessage(int event,String name,uint64_t waittime) {
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

_ConfirmFenceMessage::_ConfirmFenceMessage() {
    this->event = 0;
    this->privateData = 0;
}

_ConfirmFenceMessage::_ConfirmFenceMessage(String name,uint64_t result) {
    this->event = ConfirmFence;
    this->fencename = name;
    this->privateData = result;
}

}
