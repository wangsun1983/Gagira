#ifndef __GAGRIA_ARCHIVE_HANDLE_RESULT_HPP__
#define __GAGRIA_ARCHIVE_HANDLE_RESULT_HPP__

#include "Object.hpp"
#include "String.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ArchiveHandleResult) {
public:
    _ArchiveHandleResult(String path,int result);
    _ArchiveHandleResult(String path);
    String path;
    int result;
};

}

#endif
