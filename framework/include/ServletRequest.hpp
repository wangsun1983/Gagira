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
#include "JsonValue.hpp"
#include "JsonReader.hpp"
#include "OStdInstanceOf.hpp"

using namespace obotcha;

namespace gagira {

template<typename T>
class _ServletContentHelper {
public:
    T get(HttpPacket packet) {
        T obj = AutoCreate<T>();
        JsonReader reader = createJsonReader(packet->getEntity()->getContent()->toString());
        JsonValue v = reader->get();
        v->reflectTo(obj);
        return obj;
    }    
};

template<>
class _ServletContentHelper<String> {
public:
    String get(HttpPacket packet) {
        return packet->getEntity()->getContent()->toString();
    }    
};

template<>
class _ServletContentHelper<ByteArray> {
public:
    ByteArray get(HttpPacket packet) {
        return packet->getEntity()->getContent();
    }    
};

DECLARE_CLASS(ServletRequest) {
public:
    _ServletRequest(HttpPacket,HttpLinker);

    //common method
    ArrayList<HttpCookie> getCookies();
    String getInetAddress();
    HttpSession getSession();

    String getUrl();

    //TODO:default is json
    template<typename T>
    T getContent() {
        _ServletContentHelper<T> helper;
        return helper.get(mPacket);
    }

private:
    HttpPacket mPacket;
    HttpLinker mLinker;
};

}
#endif
