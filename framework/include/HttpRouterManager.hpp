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
    DefRet(HttpRouter,HashMap<String,String>) getRouter(int method, String);

  private:
    _HttpRouterManager();
    static sp<_HttpRouterManager> mInstance;

    HttpRouterMap mMaps[static_cast<int>(st(HttpMethod)::Id::Max)];
};

} // namespace gagira

#endif
