#include "ArchiveInputStream.hpp"
#include "System.hpp"

namespace gagira {

_ArchiveInputStream::_ArchiveInputStream(uint64_t fileno,ArchiveConnection c) {
    mFileNo = fileno,
    mConnection = c;
}

long _ArchiveInputStream::read(ByteArray data) {
    return read(data,0);
}

long _ArchiveInputStream::read(ByteArray data, uint64_t start) {
    return read(data,start,data->size() - start);
}

long _ArchiveInputStream::read(ByteArray data, uint64_t start,uint64_t length) {
    auto result = mConnection->read(mFileNo,length - start);
    int len = std::min(result->size(),data->size());
    memcpy(data->toValue() + start,result->toValue(),len);
    return len;
}

ByteArray _ArchiveInputStream::readAll() {
    //TODO
    return nullptr;
}

long _ArchiveInputStream::seekTo(int index) {
    printf("ArchiveInputStream file no is %d,index is %d \n",mFileNo,index);
    return mConnection->seekTo(mFileNo,index,st(ApplySeekToMessage)::Read);
}

void _ArchiveInputStream::close() {
    //mConnection->close();
    mConnection->closeStream(mFileNo);
}

bool _ArchiveInputStream::open() {
    //do nothing
    return true;
}


}

