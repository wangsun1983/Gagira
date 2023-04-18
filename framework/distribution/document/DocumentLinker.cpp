#include "DocumentLinker.hpp"
#include "FileInputStream.hpp"

using namespace obotcha;

namespace gagira {

_DocumentLinker::_DocumentLinker(Socket sock,int buffsize):_DistributeLinker(sock,buffsize) {
}

_DocumentLinker * _DocumentLinker::setFileSize(uint64_t length) {
    mFileSize = length;
    return this;
}

_DocumentLinker * _DocumentLinker::setPath(String path) {
    mPath = path;
    return this;
}

OutputStream _DocumentLinker::getOutputStream() {
    if(mOutputStream == nullptr) {
        File file = createFile(mPath);
        if(!file->exists()) {
            file->createNewFile();
        }

        mOutputStream = createFileOutputStream(file);
        mOutputStream->open();
    }
    return mOutputStream;
}

uint64_t _DocumentLinker::reduceFileSize(uint32_t length) {
    mFileSize -= length;
    return mFileSize;
}

_DocumentLinker::~_DocumentLinker() {
    if(mOutputStream != nullptr) {
        mOutputStream->close();
        mOutputStream = nullptr;
    }
}

void _DocumentLinker::setStatus(int status) {
    mStatus = status;
}

int _DocumentLinker::getStatus() {
    return mStatus;
}


}

