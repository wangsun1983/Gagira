#include "ArchiveHandleResult.hpp"
#include "DistributeHandler.hpp"

namespace gagira {

_ArchiveHandleResult::_ArchiveHandleResult(String path):_ArchiveHandleResult(path,st(DistributeHandler)::Deal) {

}

_ArchiveHandleResult::_ArchiveHandleResult(String path,int result) {
    this->path = path;
    this->result = result;
}

}

