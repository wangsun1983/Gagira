#ifndef __GAGIRA_HTTP_RESOURCE_MANAGER_HPP__
#define __GAGIRA_HTTP_RESOURCE_MANAGER_HPP__

#include <mutex>
#include <thread>

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "File.hpp"
#include "HashMap.hpp"
#include "ReadWriteLock.hpp"
#include "String.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HttpResourceManager) {
  public:
    static sp<_HttpResourceManager> getInstance();
    void setResourceDir(String dir);

    File findResource(String url);
    void setViewRedirect(String segment, String filename);

  private:
    _HttpResourceManager();
    static sp<_HttpResourceManager> mInstance;

    HashMap<String, String> mRedirectMaps;

    String mResourceDir;
    HashMap<String, File> resourceCaches;

    ReadLock mReadLock;
    WriteLock mWriteLock;
    ReadWriteLock mReadWriteLock;
};

} // namespace gagira

#endif
