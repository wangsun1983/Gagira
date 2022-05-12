#ifndef __MY_GROUP_TEXT_MESSAGE_DATA_HPP__
#define __MY_GROUP_TEXT_MESSAGE_DATA_HPP__

#include "BaseNetData.hpp"
#include "Reflect.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(TextMessageData) IMPLEMENTS(BaseNetData) {
public:
    String username;
    String groupname;
    String data;
    DECLARE_REFLECT_FIELD(TextMessageData,username,groupname,data,type)
};

#endif
