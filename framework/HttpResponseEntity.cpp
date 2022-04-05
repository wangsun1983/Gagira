
#include "HttpResponseEntity.hpp"

namespace gagira {

int _HttpResponseEntity::getStatus() { 
    return mStatus; 
}

ArrayList<HttpCookie> _HttpResponseEntity::getCookies() { 
    return mCookies; 
}

TextContent _HttpResponseEntity::getContent() { 
    return mContent; 
}

} // namespace gagira
