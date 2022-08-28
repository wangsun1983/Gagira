#ifndef __GAGRIA_MQ_DEFAULT_PERSISTENCE_HPP__
#define __GAGRIA_MQ_DEFAULT_PERSISTENCE_HPP__

#include "String.hpp"
#include "MqMessage.hpp"
#include "MqPersistenceInterface.hpp"
#include "OStdReturnValue.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqDefaultPersistenceData) {
public:
    _MqDefaultPersistenceData(ByteArray,int);
    ByteArray data;
    int flags;
};

DECLARE_CLASS(MqDefaultPersistence) IMPLEMENTS(MqPersistenceInterface) {
public:
    int init();
    String onNewMessage(String channel,ByteArray msg,int flags);
    String onFail(String channel,ByteArray msg,int flags);
    DefRet(String,ByteArray,int) onTakeMessage(String channel);
    DefRet(String,ByteArray,int) onTakeFailMessage(String channel);
    int onRemove(String channel,String token);

private:
    Mutex mDataMutex;
    HashMap<String,LinkedList<MqDefaultPersistenceData>> mMessages;

    Mutex mFailMsgMutex;
    HashMap<String,LinkedList<MqDefaultPersistenceData>> mFailMessages;

    int _remove(String channel,String token,Mutex,HashMap<String,LinkedList<MqDefaultPersistenceData>>);
};

}
#endif
