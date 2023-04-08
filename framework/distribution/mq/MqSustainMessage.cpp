#include "MqSustainMessage.hpp"

using namespace obotcha;

namespace gagira {

_MqSustainMessage::_MqSustainMessage() {
    mCode = 0;
}

_MqSustainMessage::_MqSustainMessage(int code,String message){
    mCode = code;
    mMessage = message;
}

int _MqSustainMessage::getCode() {
    return mCode;
}

String _MqSustainMessage::getMessage() {
    return mMessage;
}

}