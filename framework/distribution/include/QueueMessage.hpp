#ifndef __GAGRIA_QUEUE_MESSAGE_HPP__
#define __GAGRIA_QUEUE_MESSAGE_HPP__

#include "Object.hpp"
#include "Serializable.hpp"
#include "ByteArray.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(QueueMessage) IMPLEMENTS(Serializable) {
public:
    enum EventType {
        None = 0,
        Acquire,
        Submit,
        Cancel,
        AckFromClient,
        AckFromServer,
        Task,
    };

    enum ResultType {
        Ok = 0,
        IlleaglMd5sum,
    };

    _QueueMessage(uint32_t event,uint32_t reqid,ByteArray data);
    _QueueMessage();
    
    uint32_t getEvent();
    void setEvent(uint32_t event);
    void setData(ByteArray);
    ByteArray getData();
    uint32_t getReqId();
    void setReqId(uint32_t);
    void setCancelId(uint32_t);
    uint32_t getCancelId();
    uint32_t getResult();
    void setResult(uint32_t result);

protected:
    uint32_t event;
    ByteArray data;
    uint32_t reqId;
    uint32_t result;
    uint32_t cancelId;

    DECLARE_REFLECT_FIELD(QueueMessage,event,data,reqId,cancelId,result);
};

DECLARE_CLASS(CancelWaitMessage) IMPLEMENTS(QueueMessage) {
public:
    _CancelWaitMessage(uint32_t reqid,uint32_t cancelid);
};

DECLARE_CLASS(MessageClientAck) IMPLEMENTS(QueueMessage) {
public:
    _MessageClientAck(uint32_t reqid);
};

DECLARE_CLASS(MessageResponse) IMPLEMENTS(QueueMessage) {
public:
    _MessageResponse();
    
    _MessageResponse(uint32_t reqid,uint32_t result,ByteArray data = nullptr);
};

}
#endif
