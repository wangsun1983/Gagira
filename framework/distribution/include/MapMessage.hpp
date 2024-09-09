#ifndef __GAGRIA_MAP_MESSAGE_HPP__
#define __GAGRIA_MAP_MESSAGE_HPP__

#include "Object.hpp"
#include "Serializable.hpp"
#include "ByteArray.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MapMessage) IMPLEMENTS(Serializable) {
public:
    enum Type {
        ConnectMessage = 0,
        Monitor,
        ResponseMonitor,
        UnMonitor,
        ResponseUnMonitor,
        Update,
        ResponseUpdate,
        Acquire,
        ResponseAcquire,
        Remove,
        ResponseRemove,
        NotifyUpdate,
        NotifyRemove,
        Disconnect,
    };

    enum ResultType {
        Ok = 0,
        IlleaglMd5sum,
    };


    _MapMessage();
    _MapMessage(uint64_t id,int event,String tag,ByteArray data,String md5sum);
    
    uint64_t getId();
    bool isNeedResponse();
    uint32_t getEvent();
    String getTag();
    ByteArray getData();
    uint32_t getResult();

protected:
    uint64_t id;
    uint32_t event;
    String tag;
    ByteArray data;
    String md5sum; //form data md5sum 
    uint32_t result;

    DECLARE_REFLECT_FIELD(MapMessage,id,event,tag,data,md5sum,result);
};

//---SapceMonitorMessage--
DECLARE_CLASS(MapMonitorMessage) IMPLEMENTS(MapMessage) {
public:
    _MapMonitorMessage(uint64_t id,ArrayList<String> list);
    _MapMonitorMessage(uint64_t id,String);
};

//---SapceUnMonitorMessage--
DECLARE_CLASS(MapUnMonitorMessage) IMPLEMENTS(MapMessage) {
public:
    _MapUnMonitorMessage(uint64_t id,ArrayList<String> list);
    _MapUnMonitorMessage(uint64_t id,String);
};

//---SapceUpdateMessage--
DECLARE_CLASS(MapUpdateMessage) IMPLEMENTS(MapMessage) {
public:
    _MapUpdateMessage(uint64_t id,String tag,ByteArray value,String md5sum);
};

//---SapceAcquireMessage--
DECLARE_CLASS(MapAcquireMessage) IMPLEMENTS(MapMessage) {
public:
    _MapAcquireMessage(uint64_t id,String);
};

//---SpaceRemoveMessage--
DECLARE_CLASS(MapRemoveMessage) IMPLEMENTS(MapMessage) {
public:
    _MapRemoveMessage(uint64_t id,String tag,String md5sum);
};

//---MapMessageResult---
DECLARE_CLASS(MapMessageResponse) IMPLEMENTS(MapMessage) {
public:
    _MapMessageResponse(uint64_t id,uint32_t event,int result,String tag,ByteArray data);
    _MapMessageResponse(uint64_t id,uint32_t event,int result);
};

}

#endif
