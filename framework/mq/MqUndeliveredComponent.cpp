#include "MqUndeliveredComponent.hpp"

using namespace obotcha;

namespace gagira {

_MqUndeliveredComponent::_MqUndeliveredComponent() {
    mMutex = createMutex();
    mDatas = createHashMap<String,LinkedList<ByteArray>>();
}

int _MqUndeliveredComponent::newChannel(String channel) {
    AutoLock l(mMutex);
    LinkedList<ByteArray> list = createLinkedList<ByteArray>();
    mDatas->put(channel,list);
    return 0;
}

String _MqUndeliveredComponent::newMessage(String channel,ByteArray msg) {
    AutoLock l(mMutex);
    LinkedList<ByteArray> list = mDatas->get(channel);
    if(list == nullptr) {
        list = createLinkedList<ByteArray>();
        mDatas->put(channel,list);
    }
    list->putLast(msg);
    return createString(msg->hashcode());
}

DefRet(String,ByteArray) _MqUndeliveredComponent::take(String channel) {
    AutoLock l(mMutex);
    LinkedList<ByteArray> list = mDatas->get(channel);
    if(list != nullptr) {
        ByteArray data = list->takeFirst();
        return MakeRet(createString(data->hashcode()),data);
    }

    return MakeRet(nullptr,nullptr);
}

int _MqUndeliveredComponent::remove(String channel,String token) {
    AutoLock l(mMutex);
    LinkedList<ByteArray> list = mDatas->get(channel);
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
        mDatas->remove(channel);
    }

    return result;
}

}
