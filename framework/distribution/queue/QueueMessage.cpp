#include "QueueMessage.hpp"
#include "ByteArrayWriter.hpp"
#include "StringBuffer.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

_QueueMessage::_QueueMessage() {
    //do nothing
}

_QueueMessage::_QueueMessage(int event,ByteArray data) {
    this->event = event;
    this->data = data;
}

}

