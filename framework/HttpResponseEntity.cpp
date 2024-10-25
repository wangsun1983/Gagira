
#include "HttpResponseEntity.hpp"

namespace gagira {

sp<_HttpResponseEntity>_HttpResponseEntity::Create(HttpChunk v,int status,ArrayList<HttpCookie> cookies) {
    auto entity = sp<_HttpResponseEntity>::New();
    entity->mChunk = v;
    entity->mStatus = status;
    entity->mCookies = cookies;
    entity->mContent = nullptr;
    return entity;
}

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

HttpChunk _HttpResponseEntity::getChunk() {
    return mChunk;
}

} // namespace gagira
