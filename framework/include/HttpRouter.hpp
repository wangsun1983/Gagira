#ifndef __OBOTCHA_HTTP_ROUTER_HPP__
#define __OBOTCHA_HTTP_ROUTER_HPP__

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "HashMap.hpp"
#include "String.hpp"
#include "TextContent.hpp"
#include "HttpResponseEntity.hpp"

namespace obotcha {

DECLARE_CLASS(RouterListener) {
  public:
    virtual HttpResponseEntity onInvoke() = 0;
};

DECLARE_CLASS(HttpRouter) {
  public:
    _HttpRouter(String p, RouterListener l);

    String getPath();
    RouterListener getListener();

  private:
    String mPath;
    RouterListener mListener;
};

} // namespace obotcha

#endif
