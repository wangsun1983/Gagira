#ifndef __GAGRIA_DISTRIBUTE_MESSAGE_CONVERTOR_HPP__
#define __GAGRIA_DISTRIBUTE_MESSAGE_CONVERTOR_HPP__

#include "Object.hpp"
#include "ByteArray.hpp"
#include "ByteRingArray.hpp"
#include "ByteRingArrayReader.hpp"
#include "ArrayList.hpp"
#include "ByteArrayWriter.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DistributeMessageConverter) {
public:
    template<typename T>
    ByteArray generatePacket(T msg) {
        ByteArray serializeData = msg->serialize();
        ByteArray finalData = createByteArray(serializeData->size() + sizeof(uint32_t));
        ByteArrayWriter writer = createByteArrayWriter(finalData);
        writer->write<uint32_t>(serializeData->size());
        writer->write(serializeData);
        return finalData;
    }

    template<typename T>
    T generateMessage(ByteArray data) {
        ByteArray msgData = createByteArray(data->toValue() + sizeof(uint32_t),data->size() - sizeof(uint32_t),true);
        auto msg = AutoCreate<T>();
        msg->deserialize(msgData);
        return msg;
    }
};

}

#endif
