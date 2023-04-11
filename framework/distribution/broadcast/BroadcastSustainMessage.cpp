#include "BroadcastSustainMessage.hpp"

using namespace obotcha;

namespace gagira {

_BroadcastSustainMessage::_BroadcastSustainMessage() {
    mCode = 0;
}

_BroadcastSustainMessage::_BroadcastSustainMessage(int code,String message){
    mCode = code;
    mMessage = message;
}

int _BroadcastSustainMessage::getCode() {
    return mCode;
}

String _BroadcastSustainMessage::getMessage() {
    return mMessage;
}

}