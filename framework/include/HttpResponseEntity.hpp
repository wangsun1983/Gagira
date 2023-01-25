#ifndef __GAGIRA_HTTP_RESPONSE_ENTITY_HPP__
#define __GAGIRA_HTTP_RESPONSE_ENTITY_HPP__

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "TextContent.hpp"
#include "HttpStatus.hpp"
#include "HttpCookie.hpp"
#include "File.hpp"
#include "HttpChunk.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HttpResponseEntity) {

public:
    static const int NoResponse = -100;
    _HttpResponseEntity(int status = st(HttpStatus)::Ok);

    template<typename U>
    _HttpResponseEntity(U v,int status = st(HttpStatus)::Ok,ArrayList<HttpCookie> cookies = nullptr) {
        mContent = createTextContent(v);
        mStatus = status;
        mCookies = cookies;
        mChunk = nullptr;
    }

    _HttpResponseEntity(HttpChunk v,int status = st(HttpStatus)::Ok,ArrayList<HttpCookie> cookies = nullptr) {
        mChunk = v;
        mStatus = status;
        mCookies = cookies;
        mContent = nullptr;
    }


    int getStatus();
    TextContent getContent();
    ArrayList<HttpCookie> getCookies();
    
    //void setChunk(File);
    //void SetChunk(ByteArray);

    HttpChunk getChunk();

private:
    int mStatus;
    TextContent mContent;
    ArrayList<HttpCookie> mCookies;

    HttpChunk mChunk;
};

}
#endif
