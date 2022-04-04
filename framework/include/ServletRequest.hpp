#ifndef __GAGRIA_HTTP_SERVLET_REQUEST_HPP__
#define __GAGRIA_HTTP_SERVLET_REQUEST_HPP__

#include "Object.hpp"
#include "StrongPointer.hpp"
#include "String.hpp"
#include "HashMap.hpp"
#include "HttpPacket.hpp"
#include "HttpLinker.hpp"
#include "InetAddress.hpp"
#include "HttpSession.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ServletRequest) {
public:
   _ServletRequest(HttpPacket,HttpLinker);

   //common method
   ArrayList<HttpCookie> getCookies();
   String getInetAddress();
   HttpSession getSession();

private:
    HttpPacket mPacket;
    HttpLinker mLinker;
};

}
#endif
