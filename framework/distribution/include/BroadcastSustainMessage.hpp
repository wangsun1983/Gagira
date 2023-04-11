#ifndef __GAGRIA_BROADCAST_SUSTAIN_MESSAGE_HPP__
#define __GAGRIA_BROADCAST_SUSTAIN_MESSAGE_HPP__

#include "String.hpp"
#include "DistributeOption.hpp"
#include "BroadcastCenter.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(BroadcastSustainMessage) IMPLEMENTS(Serializable) {
public:
    enum Code {
        WaitForIniting = 0,
        WaitForPostBack,
        WaitForOffloading
    };

    _BroadcastSustainMessage();
    _BroadcastSustainMessage(int code,String message = nullptr);
    int getCode();
    String getMessage();

private:
    int mCode;
    String mMessage;

public:
    DECLARE_REFLECT_FIELD(BroadcastSustainMessage,mCode,mMessage);
};

}

#endif
