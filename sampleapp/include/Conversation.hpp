#ifndef __MY_GROUP_CONVERSATION_INFO_HPP__
#define __MY_GROUP_CONVERSATION_INFO_HPP__

#include "String.hpp"
#include "Reflect.hpp"

using namespace obotcha;

DECLARE_CLASS(Conversation) {
public:
    long system_time;
    String from;
    String msg_type;
    String data;

    DECLARE_REFLECT_FIELD(Conversation,system_time,from,msg_type,data)
};


#endif