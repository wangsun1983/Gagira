#ifndef __GAGIRA_WEBSOCKET_ROUTER_MANAGER_HPP__
#define __GAGIRA_WEBSOCKET_ROUTER_MANAGER_HPP__

#include <mutex>
#include <thread>

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "HashMap.hpp"
#include "HttpMethod.hpp"
#include "HttpRouterMap.hpp"
#include "String.hpp"
#include "ReadWriteLock.hpp"
#include "HttpResponseEntity.hpp"
#include "Controller.hpp"

using namespace obotcha;

namespace gagira {

using WsResponseEntity = HttpResponseEntity;

DECLARE_CLASS(WebSocketRouterManager) {
  public:
      static sp<_WebSocketRouterManager> getInstance();
      void addRouter(String,ControllerRouter);  
      ControllerRouter getRouter(String);

  private:
      _WebSocketRouterManager();
      static sp<_WebSocketRouterManager> mInstance;

      ReadWriteLock lock;
      ReadLock mReadLock;
      WriteLock mWriteLock;
      HashMap<String,ControllerRouter> mControllers;

};

} // namespace gagira

#endif
