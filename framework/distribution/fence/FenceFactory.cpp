#include "FenceFactory.hpp"

namespace gagira {

_FenceFactory::_FenceFactory(String url) {
    mUrl = url;
}

FenceMutex _FenceFactory::getFenceMutex(String name) {
    auto connection = FenceConnection::New(mUrl);
    Inspect(connection->connect() < 0,nullptr)

    return FenceMutex::New(connection,name);
}

FenceReadWriteLock _FenceFactory::getFenceReadWriteLock(String name) {
    auto connection = FenceConnection::New(mUrl);
    Inspect(connection->connect() < 0,nullptr)
    return FenceReadWriteLock::New(connection,name);
}

}