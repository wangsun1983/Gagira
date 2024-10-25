#ifndef __GAGIRA_WEBSOCKET_RESPONSE_ENTITY_HPP__
#define __GAGIRA_WEBSOCKET_RESPONSE_ENTITY_HPP__

#include "Object.hpp"
#include "ByteArray.hpp"
#include "String.hpp"
#include "ResponseEntity.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(WsResponseEntity) IMPLEMENTS(ResponseEntity) {
public:
    static sp<_WsResponseEntity> Create(ByteArray);
    static sp<_WsResponseEntity> Create(String);

    ByteArray getBinaryData();
    String getTextData();
private:
    ByteArray mData;
    String mText;
};

} // namespace gagira

#endif
