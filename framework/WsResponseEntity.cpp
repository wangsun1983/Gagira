#include "WsResponseEntity.hpp"

namespace gagira {

WsResponseEntity _WsResponseEntity::Create(ByteArray data) {
    auto entity = WsResponseEntity::New();
    entity->mData = data;
    return entity;
}

WsResponseEntity _WsResponseEntity::Create(String text) {
    auto entity = WsResponseEntity::New();
    entity->mText = text;
    return entity;
}

ByteArray _WsResponseEntity::getBinaryData() {
    return mData;
}

String _WsResponseEntity::getTextData() {
    return mText;
}

}