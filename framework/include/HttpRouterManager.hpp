#ifndef __GAGIRA_HTTP_ROUTER_MANAGER_HPP__
#define __GAGIRA_HTTP_ROUTER_MANAGER_HPP__

#include <mutex>
#include <thread>

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "HashMap.hpp"
#include "HttpMethod.hpp"
#include "HttpRouterMap.hpp"
#include "String.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HttpRouterManager) {
  public:
    static sp<_HttpRouterManager> getInstance();
    void addRouter(int method, HttpRouter);
    HttpRouter getRouter(int method, String, HashMap<String, String> &);

  private:
    _HttpRouterManager();
    static sp<_HttpRouterManager> mInstance;

    HttpRouterMap mMaps[st(HttpMethod)::Max];
};

} // namespace gagira

#endif
