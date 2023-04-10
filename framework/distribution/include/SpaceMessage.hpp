#ifndef __GAGRIA_SPACE_MESSAGE_HPP__
#define __GAGRIA_SPACE_MESSAGE_HPP__

#include "Object.hpp"
#include "Serializable.hpp"
#include "ByteArray.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(SpaceMessage) IMPLEMENTS(Serializable) {
public:
    enum Type {
        ConnectMessage = 0,
        Monitor,
        UnMonitor,
        Update,
        Acquire,
        Remove,
        NotifyUpdate,
        NotifyRemove,
    };

    enum ResultType {
        Ok = 0,
        IlleaglMd5sum,
    };

    uint32_t event;
    String tag;
    ByteArray data;
    String md5sum; //form data md5sum 

    _SpaceMessage();
    _SpaceMessage(int event,String tag,ByteArray data,String md5sum);

    DECLARE_REFLECT_FIELD(SpaceMessage,event,tag,data,md5sum);
};

//---SapceMonitorMessage--
DECLARE_CLASS(SpaceMonitorMessage) IMPLEMENTS(SpaceMessage) {
public:
    _SpaceMonitorMessage(ArrayList<String> list);
    _SpaceMonitorMessage(String);
};

//---SapceUnMonitorMessage--
DECLARE_CLASS(SpaceUnMonitorMessage) IMPLEMENTS(SpaceMessage) {
public:
    _SpaceUnMonitorMessage(ArrayList<String> list);
    _SpaceUnMonitorMessage(String);
};

//---SapceUpdateMessage--
DECLARE_CLASS(SpaceUpdateMessage) IMPLEMENTS(SpaceMessage) {
public:
    _SpaceUpdateMessage(String tag,ByteArray value,String md5sum);
};

//---SapceAcquireMessage--
DECLARE_CLASS(SpaceAcquireMessage) IMPLEMENTS(SpaceMessage) {
public:
    _SpaceAcquireMessage(String);
};

//---SpaceRemoveMessage--
DECLARE_CLASS(SpaceRemoveMessage) IMPLEMENTS(SpaceMessage) {
public:
    _SpaceRemoveMessage(String tag,String md5sum);
};

//---SpaceMessageResult---
DECLARE_CLASS(SpaceMessageResult) IMPLEMENTS(Serializable) {
public:
    int result;
    ByteArray data;
    DECLARE_REFLECT_FIELD(SpaceMessageResult,result,data);
};

//---SpaceAcquireMessageResult---
DECLARE_CLASS(SpaceAcquireMessageResult) IMPLEMENTS(SpaceMessageResult) {
public:
    _SpaceAcquireMessageResult(int result,ByteArray data);
};

//---SpaceUpdateMessageResult---
DECLARE_CLASS(SpaceUpdateMessageResult) IMPLEMENTS(SpaceMessageResult) {
public:
    _SpaceUpdateMessageResult(int result,ByteArray data);
};


//---SpaceNotifyMessage---
DECLARE_CLASS(SpaceNotifyMessage) IMPLEMENTS(SpaceMessage) {
public:
    _SpaceNotifyMessage(int event,String tag,ByteArray data);
};

}

#endif
