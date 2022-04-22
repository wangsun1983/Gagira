#include "MqMessage.hpp"
#include "ByteArrayWriter.hpp"

namespace gagira {

_MqMessage::_MqMessage() {
    //Do nothing
}

_MqMessage::_MqMessage(int type,String channel,ByteArray data) {
    this->data = data;
    this->type = type;
    this->channel = channel;
}

ByteArray _MqMessage::toByteArray() {
    ByteArray serializeData = serialize();
    ByteArray finalData = createByteArray(serializeData->size() + 4);
    ByteArrayWriter writer = createByteArrayWriter(finalData);
    if(data != nullptr) {
        printf("data size is %d,serializeData size is %d \n",data->size(),serializeData->size());
    }
    writer->writeInt(serializeData->size());
    writer->writeByteArray(serializeData);

    return finalData;
}

int _MqMessage::getType() {
    return type;
}

ByteArray _MqMessage::getData() {
    return data;
}

String _MqMessage::getChannel() {
    return channel;
}

}
