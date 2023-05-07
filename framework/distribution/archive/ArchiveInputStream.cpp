#include "ArchiveInputStream.hpp"
#include "System.hpp"

namespace gagira {

 _ArchiveInputStream::_ArchiveInputStream(ArchiveFile file) {
    mArchiveFile = file;
    mConnection = createArchiveConnection(file->getRemoteUrl());
 }

_ArchiveInputStream::_ArchiveInputStream(String filename,String url) {
    mArchiveFile = createArchiveFile(filename,url);
    mConnection = createArchiveConnection(url);
}

long _ArchiveInputStream::read(ByteArray data) {
    return read(data,0);
}

long _ArchiveInputStream::read(ByteArray data, int start) {
    return read(data,start,data->size() - start);
}

long _ArchiveInputStream::read(ByteArray data, int start,int length) {
    auto result = mConnection->read(length - start);
    int len = std::min(result->size(),data->size());
    memcpy(data->toValue() + start,result->toValue(),len);
    return len;
}

ByteArray _ArchiveInputStream::readAll() {
    return nullptr;
}

bool _ArchiveInputStream::open() {
    Inspect(mConnection->connect() < 0,false) ;
    return mConnection->open(mArchiveFile->getFileName(),O_RDONLY) == 0;
}


long _ArchiveInputStream::seekTo(int index) {
    return mConnection->seekTo(index);
}

void _ArchiveInputStream::close() {
    mConnection->close();
}


}

