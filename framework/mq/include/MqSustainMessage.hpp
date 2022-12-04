#ifndef __GAGRIA_MQ_SUSTAIN_MESSAGE_HPP__
#define __GAGRIA_MQ_SUSTAIN_MESSAGE_HPP__

#include "String.hpp"
#include "MqOption.hpp"
#include "MqCenter.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqSustainMessage) IMPLEMENTS(Serializable) {
public:
    enum Code {
        WaitForIniting = 0,
        WaitForPostBack,
        WaitForOffloading
    };

    _MqSustainMessage();
    _MqSustainMessage(int code,String message = nullptr);
    int getCode();
    String getMessage();

private:
    int mCode;
    String mMessage;

public:
    DECLARE_REFLECT_FIELD(MqSustainMessage,mCode,mMessage);
};

}

#endif
