#ifndef __GAGRIA_MQ_MESSAGE_HPP__
#define __GAGRIA_MQ_MESSAGE_HPP__

#include "String.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "Reflect.hpp"
#include "Serializable.hpp"
#include "String.hpp"
#include "Serializable.hpp"

using namespace obotcha;

namespace gagira {

class _MqWorker;
class _MqCenter;

DECLARE_CLASS(MqMessage) IMPLEMENTS(Serializable){
public:
    friend class _MqWorker;
    friend class _MqCenter;
    enum MessageType {
        ShakeHand = 0,
        Subscribe,
        UnSubscribe,
        Publish,
        PublishStick,
    };
    _MqMessage();
    _MqMessage(int type,String channel,ByteArray data);
    
    ByteArray toByteArray();

    int getType();
    ByteArray getData();
    String getChannel();

private:
    int type;
    ByteArray data;
    String channel;

    Socket mSocket;
    ByteArray mSerializableData;

public:
    DECLARE_REFLECT_FIELD(MqMessage,type,channel,data);
};


}
#endif
