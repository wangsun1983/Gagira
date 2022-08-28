#include "MqDefaultPersistence.hpp"

using namespace obotcha;

namespace gagira {

///////-------------------
_MqDefaultPersistenceData::_MqDefaultPersistenceData(ByteArray data,int flags) {
    this->data = data;
    this->flags = flags;
}

///////-------------------
int _MqDefaultPersistence::init() {
    mDataMutex = createMutex();
    mMessages = createHashMap<String,LinkedList<MqDefaultPersistenceData>>();

    mFailMsgMutex = createMutex();
    mFailMessages = createHashMap<String,LinkedList<MqDefaultPersistenceData>>();

    return 0;
}

String _MqDefaultPersistence::onNewMessage(String channel,ByteArray msg,int flags) {
    auto data = createMqDefaultPersistenceData(msg,flags);

    AutoLock l(mDataMutex);
    LinkedList<MqDefaultPersistenceData> list = mMessages->get(channel);
    if(list == nullptr) {
        list = createLinkedList<MqDefaultPersistenceData>();
        mMessages->put(channel,list);
    }
    list->putLast(data);
    return createString(data->hashcode());
}

String _MqDefaultPersistence::onFail(String channel,ByteArray msg,int flags) {
    auto data = createMqDefaultPersistenceData(msg,flags);

    AutoLock l(mFailMsgMutex);
    auto list = mFailMessages->get(channel);
    if(list == nullptr) {
        list = createLinkedList<MqDefaultPersistenceData>();
        mFailMessages->put(channel,list);
    }
    list->putLast(data);
    return createString(data->hashcode());
}

DefRet(String,ByteArray,int) _MqDefaultPersistence::onTakeMessage(String channel) {
    AutoLock l(mDataMutex);
    auto list = mMessages->get(channel);
    if(list != nullptr) {
        MqDefaultPersistenceData data = list->takeFirst();
        return MakeRet(createString(data->hashcode()),data->data,data->flags);
    }

    return MakeRet(nullptr,nullptr,-1);
}

DefRet(String,ByteArray,int) _MqDefaultPersistence::onTakeFailMessage(String channel) {
    AutoLock l(mFailMsgMutex);
    auto list = mFailMessages->get(channel);
    if(list != nullptr) {
        MqDefaultPersistenceData data = list->takeFirst();
        if(data != nullptr) {
            return MakeRet(createString(data->hashcode()),data->data,data->flags);
        }
    }

    return MakeRet(nullptr,nullptr,-1);
}

int _MqDefaultPersistence::onRemove(String channel,String token) {
    if(_remove(channel,token,mDataMutex,mMessages) != -1) {
        return 0;
    }

    if(_remove(channel,token,mFailMsgMutex,mFailMessages) != -1) {
        return 0;
    }

    return -1;
}

int _MqDefaultPersistence::_remove(String channel,
                                    String token,
                                    Mutex mutex,
                                    HashMap<String,LinkedList<MqDefaultPersistenceData>> messages) {
    AutoLock l(mutex);
    auto list = messages->get(channel);
    if(list == nullptr) {
        return 0;
    }

    auto iterator = list->getIterator();

    uint64_t hashCode = token->toBasicUint64();
    int result = -1;

    while(iterator->hasValue()) {
        auto value = iterator->getValue();
        if(value->hashcode() == hashCode) {
            iterator->remove();
            result = 0;
            break;
        }
        iterator->next();
    }

    if(list->size() == 0) {
        messages->remove(channel);
    }

    return result;                                    
}



}
