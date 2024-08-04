#ifndef __GAGRIA_ARCHIVE_OUTPUTSTREAM_HPP__
#define __GAGRIA_ARCHIVE_OUTPUTSTREAM_HPP__

#include "Object.hpp"
#include "ArchiveFile.hpp"
#include "OutputStream.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ArchiveOutputStream) IMPLEMENTS(OutputStream) {
public:
    _ArchiveOutputStream(uint64_t fileno,ArchiveConnection connection);
    
    long write(char c);
    long write(ByteArray buff);
    long write(ByteArray buff, uint64_t start);
    long write(ByteArray buff, uint64_t start, uint64_t len);
    bool open();
    bool open(int);
    void close();
    void flush();
    void setAsync(bool,sp<_AsyncOutputChannelPool> pool = nullptr);

private:
    ArchiveConnection mConnection;
    uint64_t mFileNo;
};

}

#endif
