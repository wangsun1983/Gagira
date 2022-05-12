#ifndef __MY_GROUP_BASE_NET_DATA_HPP__
#define __MY_GROUP_BASE_NET_DATA_HPP__

#include "GroupInfo.hpp"
#include "String.hpp"
#include "MySqlDao.hpp"
#include "ArrayList.hpp"
#include "Conversation.hpp"

using namespace obotcha;
using namespace gagira;

enum MessageType {
    ShakeHand = 0,
    TextMessage,
};

DECLARE_CLASS(BaseNetData) {
public:
    int type;
};

#endif
