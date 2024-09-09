#include <signal.h>

#include "ArchiveConnection.hpp"
#include "Inet4Address.hpp"
#include "Inet6Address.hpp"
#include "SocketBuilder.hpp"
#include "FileInputStream.hpp"
#include "HttpUrl.hpp"
#include "InitializeException.hpp"
#include "ArchiveMessage.hpp"
#include "FileOutputStream.hpp"
#include "ArchiveFileManager.hpp"
#include "Md.hpp"
#include "Defer.hpp"
#include "ForEveryOne.hpp"

using namespace obotcha;

namespace gagira {

#define PROGRESS_CALL(status,progress) \
    if(func != nullptr) {                   \
        func(status,progress);              \
    }

_ArchiveConnection::_ArchiveConnection(String s) {
    signal(SIGPIPE, SIG_IGN); //ignore SIGPIPE
    HttpUrl url = HttpUrl::New(s);
    mMutex = Mutex::New();

    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find ArchiveCenter");
    }

    mConverter = DistributeMessageConverter::New();
    mTemporaryUploadSocks = ConcurrentQueue<Socket>::New();

    mErrno = 0;
}

_ArchiveConnection::~_ArchiveConnection() {
    close();
}

uint64_t _ArchiveConnection::querySize(String filename) {
    AutoLock l(mMutex);
    mErrno = -EINVAL;
    uint64_t fileSize = 0;
    Inspect(mOutput == nullptr,fileSize);

    do {
        mErrno = 0;
        QueryInfoMessage msg = QueryInfoMessage::New(filename);
        if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }

        ConfirmQueryInfoMessage response = waitResponse<ConfirmQueryInfoMessage>(mInput);
        if(response == nullptr) {
            break;
        } else if(!response->isPermitted()) {
            mErrno = -response->getPermitFlag();
        } else {
            fileSize = response->getQueryFileSize();
        }
    } while(0);

    return fileSize;
}

/*
 * @param (FileNo,Result)
 *         FileNo:open file idResult:open result.
 */
DefRet(uint64_t,int) _ArchiveConnection::openStream(String filename,uint64_t flags) {
    AutoLock l(mMutex);
    mErrno = -EINVAL;

    Inspect(mOutput == nullptr,MakeRet(0,mErrno));
    ConfirmOpenMessage response = nullptr;
    
    do {
        mErrno = 0;
        auto req = ApplyOpenMessage::New(filename,flags);
        if(mOutput->write(mConverter->generatePacket(req)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }
        
        response = waitResponse<ConfirmOpenMessage>(mInput);
        if(response != nullptr && !response->isPermitted()) {
            mErrno = -response->getPermitFlag();
        }
    } while(0);

    return MakeRet(response->getFileNo(),mErrno);
}

int _ArchiveConnection::closeStream(uint64_t fileno) {
    AutoLock l(mMutex);
    mErrno = -EINVAL;
    Inspect(mOutput == nullptr,mErrno);

    do {
        mErrno = 0;
        auto req = ApplyCloseStreamMessage::New(fileno);
        if(mOutput->write(mConverter->generatePacket(req)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }

        auto response = waitResponse<ConfirmCloseStreamMessage>(mInput);
        if(response != nullptr && !response->isPermitted()) {
            mErrno = -response->getPermitFlag();
        }
    } while(0);

    return mErrno;
}

ByteArray _ArchiveConnection::read(uint64_t fileno,uint64_t length) {
    AutoLock l(mMutex);
    mErrno = -EINVAL;
    Inspect(mOutput == nullptr,nullptr);
    ByteArray data = nullptr;

    do {
        mErrno = 0;
        ApplyReadMessage msg = ApplyReadMessage::New(fileno,length);
        if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }
        
        auto response = waitResponse<ConfirmReadMessage>(mInput);
        if(response == nullptr) {
            break;
        } else if(!response->isPermitted()) {
            mErrno = -response->getPermitFlag();
        } else {
            data = response->getData();
        }

    } while(0);

    return data;
}

int _ArchiveConnection::write(uint64_t fileno,ByteArray data) {
    AutoLock l(mMutex);
    mErrno = -EINVAL;
    Inspect(mOutput == nullptr,mErrno);

    do {
        mErrno = 0;
        ApplyWriteMessage msg = ApplyWriteMessage::New(fileno,data);
        if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }

        auto response = waitResponse<ConfirmWriteMessage>(mInput);
        if(response != nullptr && !response->isPermitted()) {
            mErrno = -response->getPermitFlag();
        }
    } while(0);

    return mErrno;
}

int _ArchiveConnection::rename(String originalname,String newname) {
    AutoLock l(mMutex);
    mErrno = -EINVAL;
    Inspect(mOutput == nullptr,mErrno);

    do {
        mErrno = 0;
        ApplyRenameMessage msg = ApplyRenameMessage::New(originalname,newname);
        if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }

        auto response = waitResponse<ConfirmRenameMessage>(mInput);
        if(response != nullptr && !response->isPermitted()) {
            mErrno = -response->getPermitFlag();
        }
    } while(0);

    return mErrno;
}

int _ArchiveConnection::remove(String filename) {
    AutoLock l(mMutex);
    mErrno = -EINVAL;
    Inspect(mOutput == nullptr,mErrno);
    
    do {
        mErrno = 0;
        ApplyDelMessage msg = ApplyDelMessage::New(filename);
        if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }
        auto response = waitResponse<ConfirmDelMessage>(mInput);
        if(response != nullptr && !response->isPermitted()) {
            mErrno = -response->getPermitFlag();
        }
    } while(0);

    return mErrno;
}

int _ArchiveConnection::seekTo(uint64_t fileno,uint32_t pos,st(ApplySeekToMessage)::Type type) {
    AutoLock l(mMutex);
    mErrno = -EINVAL;
    Inspect(mOutput == nullptr,mErrno);
    do {
        mErrno = 0;
        ApplySeekToMessage msg = ApplySeekToMessage::New(fileno,pos,type);
        if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }

        auto response = waitResponse<ConfirmSeekToMessage>(mInput);
        if(response != nullptr && !response->isPermitted()) {
            mErrno = -response->getPermitFlag();
        }
    } while(0);

    return mErrno;
}

int _ArchiveConnection::download(String filename,String savepath,std::function<void(int,int)> func) {
    AutoLock l(mMutex);
    mErrno = -EINVAL;
    Inspect(mOutput == nullptr,mErrno);
    do {
        mErrno = 0;
        PROGRESS_CALL(ProcessStatus::Start,0)
        ApplyDownloadMessage msg = ApplyDownloadMessage::New(filename);
        if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }
        PROGRESS_CALL(ProcessStatus::FinishSendReq,0)

        ConfirmDownloadMessage resp = waitResponse<ConfirmDownloadMessage>(mInput);
        if(resp == nullptr) {
            break;
        }

        if(!resp->isPermitted()) {
            mErrno = -resp->getPermitFlag();
            break;
        }

        if(resp->getDownloadFileSize() == 0) {
            mErrno = -ENOENT;
            break;
        }
        auto verifyData = resp->getVerifyData();

        uint64_t filesize = resp->getDownloadFileSize();
        ProcessDownloadMessage processReq = ProcessDownloadMessage::New(resp->getFileNo());
        if(mOutput->write(mConverter->generatePacket(processReq)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }
        
        PROGRESS_CALL(ProcessStatus::StartDownloading,0)
        File file = File::New(savepath);
        if(!file->exists()) {
            file->createNewFile();
        }

        ByteArray data = ByteArray::New(32*1024);
        FileOutputStream outPut = FileOutputStream::New(file);
        outPut->open();
        size_t recvsize = 0;
        size_t totalsize = filesize;
        
        while(filesize != 0) {
            int len = mInput->read(data);
            if(len <= 0) {
                break;
            }
            data->quickShrink(len);
            outPut->write(data);
            data->quickRestore();
            PROGRESS_CALL(ProcessStatus::StartUploading,(recvsize*100)/totalsize)
            filesize -= len;
        }
        outPut->close();
        
        if(filesize != 0) {
            mErrno = -ENETUNREACH;
            break;
        }
        //check verify data
        Md md5sum = Md::New();
        auto md5value = md5sum->encodeFile(file);
        if(!md5value->sameAs(verifyData->toString())) {
            mErrno = -EBADF;
            break;
        }
        PROGRESS_CALL(ProcessStatus::Finish,0)
    } while(0);

    //remove download file
    if(mErrno != 0) {
        File file = File::New(savepath);
        file->removeAll();
    }

    return mErrno;
}

int _ArchiveConnection::upload(File file,std::function<void(int,int)> func) {
    AutoLock l(mMutex);
    mErrno = -EINVAL;
    Inspect(mOutput == nullptr,mErrno);
    do {
        mErrno = 0;
        PROGRESS_CALL(ProcessStatus::Start,0)
        auto msg = ApplyUploadConnectMessage::New();
        if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
            mErrno = -ENETUNREACH;
            break;
        }
        ConfirmUploadConnectMessage resp = waitResponse<ConfirmUploadConnectMessage>(mInput);
        if(resp == nullptr) {
            break;
        }

        if(!resp->isPermitted()) {
            mErrno = -resp->getPermitFlag();
            break;
        }

        int port = resp->getPort();
        InetAddress uploadAddress = nullptr;
        switch(mAddress->getFamily()) {
            case st(Net)::Family::Ipv4:
                uploadAddress = Inet4Address::New(mAddress->getAddress(),port);
            break;

            case st(Net)::Family::Ipv6:
                uploadAddress = Inet6Address::New(mAddress->getAddress(),port);
            break;
        }

        Socket uploadSocket = SocketBuilder::New()->setAddress(uploadAddress)->newSocket();
        mTemporaryUploadSocks->add(uploadSocket);

        Defer removeBeforeExit([this,uploadSocket] {
            mTemporaryUploadSocks->remove(uploadSocket);
        });
        if(uploadSocket->connect() < 0) {
            uploadSocket->close();
            mErrno = -ENETUNREACH;
            break;
        }
        auto uploadOutput= uploadSocket->getOutputStream();
        auto uploadInput = uploadSocket->getInputStream();
        auto applyInfo = ApplyUploadMessage::New(file);
        int ret = uploadOutput->write(mConverter->generatePacket(applyInfo));
        PROGRESS_CALL(ProcessStatus::FinishSendReq,0)
        ConfirmApplyUploadMessage confirmResp = waitResponse<ConfirmApplyUploadMessage>(uploadInput);
        
        if(confirmResp == nullptr) {
            uploadSocket->close();
            mErrno = -ENETUNREACH;
            break;
        } else if(!confirmResp->isPermitted()) {
            uploadSocket->close();
            mErrno = -confirmResp->getPermitFlag();
            break;
        }
        PROGRESS_CALL(ProcessStatus::StartUploading,0)
        
        FileInputStream inputStream = FileInputStream::New(file);
        inputStream->open();
        
        ByteArray data = ByteArray::New(32*1024);
        size_t totalsize = file->length();
        size_t sendsize = 0;
        while(1) {
            int len = inputStream->read(data);
            if(len <= 0) {
                break;
            }
            sendsize += len;
            PROGRESS_CALL(ProcessStatus::StartUploading,(sendsize*100)/totalsize)
            data->quickShrink(len);
            if(uploadOutput->write(data) <= 0) {
                uploadOutput->close();
                mErrno = -ENETUNREACH;
                break;
            }
            data->quickRestore();
        }

        if(mErrno == 0) {
            PROGRESS_CALL(ProcessStatus::WaitResponse,0)
            CompleteUploadMessage completeResponse = waitResponse<CompleteUploadMessage>(uploadInput);
            inputStream->close();
            uploadSocket->close();
            if(completeResponse == nullptr) {
                break;
            }
            PROGRESS_CALL(ProcessStatus::Finish,0)
            mErrno = -completeResponse->getPermitFlag();
        }
    } while(0);

    return mErrno;
}

int _ArchiveConnection::connect() {
    mSock = SocketBuilder::New()->setAddress(mAddress)->newSocket();
    mErrno = -ENETUNREACH;
    Inspect(mSock->connect() < 0,mErrno);

    mErrno = 0;
    mInput = mSock->getInputStream();
    mOutput = mSock->getOutputStream();
    return mErrno;
}

int _ArchiveConnection::close() {
    AutoLock l(mMutex);
    Inspect(mSock == nullptr,0);

    if(mInput != nullptr) {
        mInput->close();
        mOutput->close();
        //mInput = nullptr;
        //mOutput = nullptr;
    }
    if(mSock != nullptr) {
        mSock->close();
        mSock = nullptr;
    }

    ForEveryOne(uploadSock,mTemporaryUploadSocks) {
        uploadSock->close();
    }
    mTemporaryUploadSocks->clear();

    mErrno = 0;
    return mErrno;
}

int _ArchiveConnection::getErr() {
    return mErrno;
}

#undef PROGRESS_CALL

}

