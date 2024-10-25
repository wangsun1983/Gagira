#ifndef __GAGIRA_HTTP_RESPONSE_ENTITY_HPP__
#define __GAGIRA_HTTP_RESPONSE_ENTITY_HPP__

#include "Object.hpp"

#include "TextContent.hpp"
#include "HttpStatus.hpp"
#include "HttpCookie.hpp"
#include "File.hpp"
#include "HttpChunk.hpp"
#include "ResponseEntity.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HttpResponseEntity) IMPLEMENTS(ResponseEntity) {

public:
    static const int NoResponse = -100;
    
    _HttpResponseEntity(int status = st(HttpStatus)::Ok);

    template<typename U> 
    static sp<_HttpResponseEntity> Create(U v,int status = st(HttpStatus)::Ok,ArrayList<HttpCookie> cookies = nullptr) {
        auto entity = sp<_HttpResponseEntity>::New();
        entity->mContent = TextContent::New(v);
        entity->mStatus = status;
        entity->mCookies = cookies;
        entity->mChunk = nullptr;
        return entity;
    }

    static sp<_HttpResponseEntity>Create(HttpChunk v,int status = st(HttpStatus)::Ok,ArrayList<HttpCookie> cookies = nullptr);

    int getStatus();
    TextContent getContent();
    ArrayList<HttpCookie> getCookies();
    HttpChunk getChunk();

private:
    int mStatus;
    TextContent mContent;
    ArrayList<HttpCookie> mCookies;
    HttpChunk mChunk;
};

}
#endif
