#ifndef __GAGIRA_HTTP_RESPONSE_ENTITY_HPP__
#define __GAGIRA_HTTP_RESPONSE_ENTITY_HPP__

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "TextContent.hpp"
#include "HttpStatus.hpp"
#include "HttpCookie.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HttpResponseEntity) {

public:
    template<typename U>
    _HttpResponseEntity(U v,int status = st(HttpStatus)::Ok,ArrayList<HttpCookie> cookies = nullptr) {
        mContent = createTextContent(v);
        mStatus = status;
        mCookies = cookies;
    }

    int getStatus();
    TextContent getContent();
    ArrayList<HttpCookie> getCookies();

private:
    int mStatus;
    TextContent mContent;
    ArrayList<HttpCookie> mCookies;
};

}
#endif
