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
    _ArchiveInputStream(ArchiveFile);
    _ArchiveInputStream(String filename,String url);

    long seekTo(int index);
    
    long read(ByteArray);
    long read(ByteArray, int start);
    long read(ByteArray, int start,int length);
    ByteArray readAll();
    bool open();
    void close();

private:
    ArchiveFile mArchiveFile;
    ArchiveConnection mConnection;
};

}

#endif
