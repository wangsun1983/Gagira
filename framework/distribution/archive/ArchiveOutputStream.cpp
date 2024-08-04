#include <algorithm>

#include "ArchiveOutputStream.hpp"

namespace gagira {

_ArchiveOutputStream::_ArchiveOutputStream(uint64_t fileno,ArchiveConnection connection) {
    mFileNo = fileno;
    mConnection = connection;
}

long _ArchiveOutputStream::write(char c) {
    ByteArray data = ByteArray::New(1);
    data[0] = c;
    return write(data);
}

long _ArchiveOutputStream::write(ByteArray buff) {
    return write(buff,0);
}

long _ArchiveOutputStream::write(ByteArray buff, uint64_t start) {
    return write(buff,start,buff->size() - start);
}

long _ArchiveOutputStream::write(ByteArray buff, uint64_t start, uint64_t len) {
    int length = std::min(buff->size() - start,(size_t)len);
    ByteArray data = ByteArray::New(&buff->toValue()[start],length);
    mConnection->write(mFileNo,data);
    return 0;
}

bool _ArchiveOutputStream::open() {
    //do nothing
    return true;
}

bool _ArchiveOutputStream::open(int type) {
    //do nothing
    return true;
}

// bool _ArchiveOutputStream::open(int type) {
//     Inspect(mConnection->connect() < 0,false) ;
//     switch(type) {
//         case st(IO)::FileControlFlags::Append:
//             return mConnection->open(mArchiveFile->getFileName(),O_WRONLY|O_APPEND) == 0;
//         break;

//         case st(IO)::FileControlFlags::Trunc:
//             return mConnection->open(mArchiveFile->getFileName(),O_WRONLY|O_TRUNC) == 0;
//         break;
//     }

//     return false;
// }

void _ArchiveOutputStream::close() {
    mConnection->closeStream(mFileNo);
}

void _ArchiveOutputStream::flush() {
    //TODO
}

void _ArchiveOutputStream::setAsync(bool,sp<_AsyncOutputChannelPool> pool) {
    //TODO
}


}
