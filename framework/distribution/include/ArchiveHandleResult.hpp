#ifndef __GAGRIA_ARCHIVE_HANDLE_RESULT_HPP__
#define __GAGRIA_ARCHIVE_HANDLE_RESULT_HPP__

#include "Object.hpp"
#include "String.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ArchiveHandleResult) {
public:
    enum Type {
        Deal = 0,
        Reject,
    };

    _ArchiveHandleResult(String path,Type result);
    _ArchiveHandleResult(String path);
    _ArchiveHandleResult(Type result);
    static sp<_ArchiveHandleResult> DisAllow();
    static sp<_ArchiveHandleResult> Allow(String path,Type result);
    String path;
    Type result;
};

}

#endif
