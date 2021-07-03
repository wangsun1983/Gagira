#include "ServletRequest.hpp"

using namespace obotcha;

namespace gagira  {

_ServletRequest::_ServletRequest(HttpPacket p,HttpLinker l) {
    this->mPacket = p;
    this->mLinker = l;
}

ArrayList<HttpCookie> _ServletRequest::getCookies() {
    HttpHeader header = mPacket->getHeader();
    if(header != nullptr) {
        return header->getCookies();
    }

    return nullptr;
}

String _ServletRequest::getInetAddress() {
    mLinker->getClientIp();
}

HttpSession _ServletRequest::getSession() {
    return mLinker->getSession();
}
}
