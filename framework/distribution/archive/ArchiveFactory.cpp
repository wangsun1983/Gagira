#include "ArchiveFactory.hpp"
#include "Log.hpp"
#include "InitializeException.hpp"

namespace gagira {

_ArchiveFactory::_ArchiveFactory(String url) {
    mConnection = ArchiveConnection::New(url);
    if(mConnection->connect() < 0) {
        Trigger(InitializeException,"connect fail")
    }
}

// int _ArchiveFactory::connect() {
//     return mConnection->connect();
// }
    
ArchiveInputStream _ArchiveFactory::openRead(String filename) {
    FetchRet(fileno,result) = mConnection->openStream(filename,st(IO)::ReadOnly);
    if(result != 0) {
        LOG(ERROR)<<"open read stream error:"<<result;
        return nullptr;
    }
    return ArchiveInputStream::New(fileno,mConnection);
}

ArchiveOutputStream _ArchiveFactory::openTruncWrite(String filename) {
    FetchRet(fileno,result) = mConnection->openStream(filename,O_WRONLY|O_TRUNC);
    if(result != 0) {
        LOG(ERROR)<<"open write trunc stream error:"<<result;
        return nullptr;
    }
    return ArchiveOutputStream::New(fileno,mConnection);
}

ArchiveOutputStream _ArchiveFactory::openAppendWrite(String filename) {
    FetchRet(fileno,result) = mConnection->openStream(filename,O_WRONLY|O_APPEND);
    if(result != 0) {
        LOG(ERROR)<<"open write trunc stream error:"<<result;
        return nullptr;
    }
    return ArchiveOutputStream::New(fileno,mConnection);
}
 
int _ArchiveFactory::upload(File file) {
    return mConnection->upload(file);
}

int _ArchiveFactory::download(String filename,String savepath) {
    return mConnection->download(filename,savepath);
}

int _ArchiveFactory::remove(String filename) {
    return mConnection->remove(filename);
}
    
int _ArchiveFactory::rename(String originalname,String newname) {
    return mConnection->rename(originalname,newname);
}

uint64_t _ArchiveFactory::querySize(String filename) {
    return mConnection->querySize(filename);    
}

}

