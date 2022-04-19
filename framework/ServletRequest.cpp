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
    return mLinker->getInetAddress()->getAddress();
}

HttpSession _ServletRequest::getSession() {
    return mLinker->getSession();
}

String _ServletRequest::getUrl() {
    return mPacket->getHeader()->getUrl();
}

}
