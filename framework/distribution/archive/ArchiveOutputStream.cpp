#include "ArchiveOutputStream.hpp"

namespace gagira {

_ArchiveOutputStream::_ArchiveOutputStream(ArchiveFile file) {
    mArchiveFile = file;
    mConnection = createArchiveConnection(file->getRemoteUrl());
}

_ArchiveOutputStream::_ArchiveOutputStream(String filename,String url) {
    mArchiveFile = createArchiveFile(filename,url);
    mConnection = createArchiveConnection(url);
}

long _ArchiveOutputStream::write(char c) {
    ByteArray data = createByteArray(1);
    data[0] = c;
    return write(data);
}

long _ArchiveOutputStream::write(ByteArray buff) {
    return write(buff,0);
}

long _ArchiveOutputStream::write(ByteArray buff, int start) {
    return write(buff,start,buff->size() - start);
}

long _ArchiveOutputStream::write(ByteArray buff, int start, int len) {
    int length = std::min(buff->size() - start,len);
    ByteArray data = createByteArray(&buff->toValue()[start],length);
    mConnection->write(data);
    return 0;
}

bool _ArchiveOutputStream::open() {
    Inspect(mConnection->connect() < 0,false) ;
    return mConnection->open(mArchiveFile->getFileName(),O_WRONLY|O_TRUNC) == 0;
}

bool _ArchiveOutputStream::open(int type) {
    Inspect(mConnection->connect() < 0,false) ;
    switch(type) {
        case st(OutputStream)::Append:
            return mConnection->open(mArchiveFile->getFileName(),O_WRONLY|O_APPEND) == 0;
        break;

        case st(OutputStream)::Trunc:
            return mConnection->open(mArchiveFile->getFileName(),O_WRONLY|O_TRUNC) == 0;
        break;
    }

    return false;
}

void _ArchiveOutputStream::close() {
    mConnection->close();
}

void _ArchiveOutputStream::flush() {
    //TODO
}

void _ArchiveOutputStream::setAsync(bool,sp<_AsyncOutputChannelPool> pool) {
    //TODO
}


}
