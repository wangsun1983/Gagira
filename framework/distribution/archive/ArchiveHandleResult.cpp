#include "ArchiveHandleResult.hpp"
#include "DistributeHandler.hpp"

namespace gagira {

_ArchiveHandleResult::_ArchiveHandleResult(String path):_ArchiveHandleResult(path,Type::Deal) {

}

_ArchiveHandleResult::_ArchiveHandleResult(String path,Type result) {
    this->path = path;
    this->result = result;
}

_ArchiveHandleResult::_ArchiveHandleResult(Type result) {
    this->result = result;
}

sp<_ArchiveHandleResult> _ArchiveHandleResult::DisAllow() {
    return ArchiveHandleResult::New(Type::Reject);
}

sp<_ArchiveHandleResult> _ArchiveHandleResult::Allow(String path,Type result) {
    return ArchiveHandleResult::New(path,result);
}

}

