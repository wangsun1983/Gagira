#include "ArchiveClient.hpp"
#include "Log.hpp"
namespace gagira {

_ArchiveClient::_ArchiveClient(String url) {
    mConnection = ArchiveConnection::New(url);
}

int _ArchiveClient::connect() {
    return mConnection->connect();
}
    
ArchiveInputStream _ArchiveClient::openRead(String filename) {
    FetchRet(fileno,result) = mConnection->openStream(filename,st(IO)::ReadOnly);
    if(result != 0) {
        LOG(ERROR)<<"open read stream error:"<<result;
        return nullptr;
    }
    return ArchiveInputStream::New(fileno,mConnection);
}

ArchiveOutputStream _ArchiveClient::openTruncWrite(String filename) {
    FetchRet(fileno,result) = mConnection->openStream(filename,O_WRONLY|O_TRUNC);
    if(result != 0) {
        LOG(ERROR)<<"open write trunc stream error:"<<result;
        return nullptr;
    }
    return ArchiveOutputStream::New(fileno,mConnection);
}

ArchiveOutputStream _ArchiveClient::openAppendWrite(String filename) {
    FetchRet(fileno,result) = mConnection->openStream(filename,O_WRONLY|O_APPEND);
    if(result != 0) {
        LOG(ERROR)<<"open write trunc stream error:"<<result;
        return nullptr;
    }
    return ArchiveOutputStream::New(fileno,mConnection);
}
 
int _ArchiveClient::upload(File file) {
    return mConnection->upload(file);
}

int _ArchiveClient::download(String filename,String savepath) {
    return mConnection->download(filename,savepath);
}

int _ArchiveClient::remove(String filename) {
    return mConnection->remove(filename);
}
    
int _ArchiveClient::rename(String originalname,String newname) {
    return mConnection->rename(originalname,newname);
}

uint64_t _ArchiveClient::querySize(String filename) {
    return mConnection->querySize(filename);    
}

}

