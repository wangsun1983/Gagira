#ifndef __GAGRIA_ARCHIVE_OUTPUTSTREAM_HPP__
#define __GAGRIA_ARCHIVE_OUTPUTSTREAM_HPP__

#include "Object.hpp"
#include "ArchiveFile.hpp"
#include "OutputStream.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ArchiveOutputStream) IMPLEMENTS(OutputStream) {
public:
    _ArchiveOutputStream(ArchiveFile);
    _ArchiveOutputStream(String filename,String url);
    long write(char c);
    long write(ByteArray buff);
    long write(ByteArray buff, int start);
    long write(ByteArray buff, int start, int len);
    bool open();
    bool open(int);
    void close();
    void flush();
    void setAsync(bool,sp<_AsyncOutputChannelPool> pool = nullptr);

private:
    ArchiveFile mArchiveFile;
    ArchiveConnection mConnection;
};

}

#endif
