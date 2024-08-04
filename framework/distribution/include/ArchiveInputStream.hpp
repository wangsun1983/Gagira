#ifndef __GAGRIA_ARCHIVE_INPUTSTREAM_HPP__
#define __GAGRIA_ARCHIVE_INPUTSTREAM_HPP__

#include "Object.hpp"
#include "ArchiveFile.hpp"
#include "InputStream.hpp"
#include "ArchiveConnection.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ArchiveInputStream) IMPLEMENTS(InputStream) {
public:
    _ArchiveInputStream(uint64_t fileno,ArchiveConnection);

    long seekTo(int index);
    long read(ByteArray);
    long read(ByteArray, uint64_t start);
    long read(ByteArray, uint64_t start,uint64_t length);
    ByteArray readAll();
    bool open();
    void close();

private:
    ArchiveConnection mConnection;
    uint64_t mFileNo;
};

}

#endif
