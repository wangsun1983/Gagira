#include "HttpResourceManager.hpp"
#include "HttpLinker.hpp"
#include "HttpPacket.hpp"
#include "HttpServer.hpp"
#include "ForEveryOne.hpp"
#include "Synchronized.hpp"

namespace gagira {

sp<_HttpResourceManager> _HttpResourceManager::mInstance;

HttpResourceManager _HttpResourceManager::getInstance() {
    static std::once_flag s_flag;
    std::call_once(s_flag, [&]() {
        _HttpResourceManager *p = new _HttpResourceManager();
        p->mInstance.set_pointer(p);
    });

    return mInstance;
}

_HttpResourceManager::_HttpResourceManager() {
    mRedirectMaps = HashMap<String, String>::New();
    resourceCaches = HashMap<String, File>::New();
    mReadWriteLock = ReadWriteLock::New();
    mWriteLock = mReadWriteLock->getWriteLock();
    mReadLock = mReadWriteLock->getReadLock();
    mInterceptors = HashMap<String,ArrayList<Interceptor>>::New();
    mSearchPaths = ArrayList<String>::New();
}

void _HttpResourceManager::addSearchPath(String path) { 
    mSearchPaths->add(path);
}

void _HttpResourceManager::setViewRedirect(String segment, String filename) {
    mRedirectMaps->put(segment, filename);
}

File _HttpResourceManager::findResource(String path) {
    File file = nullptr;
    //check wheteher it is a redirect    
    if(path->indexOf(".") == -1) {
        int start = path->lastIndexOf("/");
        if(start >= 0 && path->size() > 1) {
            path = path->subString(start + 1,path->size() - start - 1);
        }
        path = mRedirectMaps->get(path);
        if(path == nullptr) {
            return nullptr;
        }
    }

    {
        AutoLock l(mReadLock);
        file = resourceCaches->get(path);
    }

    if(file == nullptr) {
        ForEveryOne(searchPath,mSearchPaths) {
            file = File::New(searchPath->append("/",path));
            if(file->exists()) {
                AutoLock l(mWriteLock);
                resourceCaches->put(path, file);
                break;
            }
            file = nullptr;
        }
    }

    //add Interceptor
    if(file != nullptr) {
        ArrayList<Interceptor> list = mInterceptors->get(path);
        if(list != nullptr) {
            auto iterator = list->getIterator();
            while(iterator->hasValue()) {
                Interceptor c = iterator->getValue();
                if(!c->onIntercept()) {
                    return nullptr;
                }
            }
        }
    }

    return file;
}

void _HttpResourceManager::addResourceInterceptor(String path,Interceptor c) {
    ArrayList<Interceptor> list = mInterceptors->get(path);
    if(list == nullptr) {
        list = ArrayList<Interceptor>::New();
        mInterceptors->put(path,list);
    }

    list->add(c);
}

} // namespace gagira