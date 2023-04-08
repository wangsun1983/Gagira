#include "SpaceMessage.hpp"
#include "ByteArrayWriter.hpp"

namespace gagira {

// sp<_SpaceMessage> _SpaceMessage::generateMessage(ByteArray data) {
//     ByteArray msgData = createByteArray(data->toValue() + sizeof(uint32_t),data->size() - sizeof(uint32_t),true);
//     SpaceMessage msg = createSpaceMessage();
//     msg->deserialize(msgData);
//     return msg;
// }

// ByteArray _SpaceMessage::generatePacket() {
//     ByteArray serializeData = serialize();
//     ByteArray finalData = createByteArray(serializeData->size() + sizeof(uint32_t));
//     ByteArrayWriter writer = createByteArrayWriter(finalData);
//     writer->write<uint32_t>(serializeData->size());
//     writer->write(serializeData);

//     return finalData;
// }

}

