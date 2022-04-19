#ifndef __GAGIRA_HTTP_ROUTER_HPP__
#define __GAGIRA_HTTP_ROUTER_HPP__

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "HashMap.hpp"
#include "String.hpp"
#include "TextContent.hpp"
#include "HttpResponseEntity.hpp"
#include "Interceptor.hpp"

namespace gagira {

DECLARE_CLASS(RouterListener) {
  public:
    virtual HttpResponseEntity onInvoke() = 0;
};

DECLARE_CLASS(HttpRouter) {
  public:
    _HttpRouter(String p, RouterListener l);

    void update(HttpRouter);

    String getPath();

    RouterListener getListener();

    HttpResponseEntity invoke();

    void addBeforeExecInterceptor(Interceptor);
    void addAfterExecInterceptor(Interceptor);

  private:
    String mPath;
    RouterListener mListener;

    ArrayList<Interceptor> beforeExecInterceptors;
    ArrayList<Interceptor> afterExecInterceptors;
    //ArrayList<Interceptor> finishRouterIntercoptors;
};

} // namespace gagira

#endif
