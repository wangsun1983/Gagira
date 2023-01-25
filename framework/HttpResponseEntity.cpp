
#include "HttpResponseEntity.hpp"

namespace gagira {

_HttpResponseEntity::_HttpResponseEntity(int status) {
    mStatus = status;
}

int _HttpResponseEntity::getStatus() { 
    return mStatus; 
}

ArrayList<HttpCookie> _HttpResponseEntity::getCookies() { 
    return mCookies; 
}

TextContent _HttpResponseEntity::getContent() { 
    return mContent; 
}

// void _HttpResponseEntity::setChunk(File file) {
//     mChunk = createHttpChunk(file);
// }

// void _HttpResponseEntity::SetChunk(ByteArray data) {
//     mChunk = createHttpChunk(data);
// }

HttpChunk _HttpResponseEntity::getChunk() {
    return mChunk;
}

} // namespace gagira
