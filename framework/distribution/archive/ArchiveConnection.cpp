#include "ArchiveConnection.hpp"
#include "Inet4Address.hpp"
#include "Inet6Address.hpp"
#include "SocketBuilder.hpp"
#include "FileInputStream.hpp"
#include "HttpUrl.hpp"
#include "InitializeException.hpp"
#include "ArchiveMessage.hpp"
#include "FileOutputStream.hpp"

using namespace obotcha;

namespace gagira {

_ArchiveConnection::_ArchiveConnection(String s) {
    HttpUrl url = createHttpUrl(s);
    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find ArchiveCenter");
    }
    mConverter = createDistributeMessageConverter();
}

_ArchiveConnection::~_ArchiveConnection() {
    close();
}

uint64_t _ArchiveConnection::querySize(String filename) {
    QueryInfoMessage msg = createQueryInfoMessage(filename);
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -ENETUNREACH;
    }

    ConfirmQueryInfoMessage resp = waitResponse<ConfirmQueryInfoMessage>(mInput);
    return resp->getQueryFileSize();
}

int _ArchiveConnection::open(String filename,uint64_t flags) {
    auto req = createApplyOpenMessage(filename,flags);
    if(mOutput->write(mConverter->generatePacket(req)) < 0) {
        return -ENETUNREACH;
    }

    auto response = waitResponse<ConfirmOpenMessage>(mInput);
    return response->isPermitted()?0:-1;
}

ByteArray _ArchiveConnection::read(uint64_t length) {
    ApplyReadMessage msg = createApplyReadMessage(length);
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return nullptr;
    }
    auto response = waitResponse<ConfirmReadMessage>(mInput);
    return response->data;
}

int _ArchiveConnection::write(ByteArray data) {
    ApplyWriteMessage msg = createApplyWriteMessage(data);
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -1;
    }

    auto response = waitResponse<ConfirmWriteMessage>(mInput);
    return response->isPermitted()?0:-1;
}

int _ArchiveConnection::rename(String originalname,String newname) {
    ApplyRenameMessage msg = createApplyRenameMessage(originalname,newname);
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -1;
    }

    auto response = waitResponse<ConfirmRenameMessage>(mInput);
    return response->isPermitted()?0:-1;
}

int _ArchiveConnection::delFile(String filename) {
    ApplyDelMessage msg = createApplyDelMessage(filename);
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -1;
    }

    auto response = waitResponse<ConfirmDelMessage>(mInput);
    return response->isPermitted()?0:-1;
}

int _ArchiveConnection::seekTo(uint32_t pos) {
    ApplySeekToMessage msg = createApplySeekToMessage(pos);
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -1;
    }

    auto response = waitResponse<ConfirmSeekToMessage>(mInput);
    return response->isPermitted()?0:-1;
}

int _ArchiveConnection::download(String filename,String savepath) {
    ApplyDownloadMessage msg = createApplyDownloadMessage(filename);
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -ENETUNREACH;
    }

    ConfirmDownloadMessage resp = waitResponse<ConfirmDownloadMessage>(mInput);
    if(resp->data != nullptr && !resp->isPermitted()) {
        return -EACCES;
    }

    if(resp->getDownloadFileSize() == 0) {
        return -ENOENT;
    }

    uint64_t filesize = resp->getDownloadFileSize();
    ProcessDownloadMessage processReq = createProcessDownloadMessage();
    if(mOutput->write(mConverter->generatePacket(processReq)) < 0) {
        return -ENETUNREACH;
    }

    File file = createFile(savepath);
    if(!file->exists()) {
        file->createNewFile();
    }

    ByteArray data = createByteArray(32*1024);
    FileOutputStream outPut = createFileOutputStream(file);
    outPut->open();
    while(filesize != 0) {
        int len = mInput->read(data);
        if(len <= 0) {
            break;
        }
        data->quickShrink(len);
        outPut->write(data);
        data->quickRestore();
        filesize -= len;
    }
    outPut->close();
    return (filesize == 0)?0:-1;
}

int _ArchiveConnection::upload(File file) {
    auto msg = createApplyUploadConnectMessage();
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -ENETUNREACH;
    }

    ConfirmUploadConnectMessage resp = waitResponse<ConfirmUploadConnectMessage>(mInput);
    int port = resp->port;
    InetAddress uploadAddress = nullptr;
    switch(mAddress->getFamily()) {
        case st(Net)::Family::Ipv4:
            uploadAddress = createInet4Address(mAddress->getAddress(),port);
        break;

        case st(Net)::Family::Ipv6:
            uploadAddress = createInet6Address(mAddress->getAddress(),port);
        break;
    }

    Socket uploadSocket = createSocketBuilder()->setAddress(uploadAddress)->newSocket();
    if(uploadSocket->connect() < 0) {
        uploadSocket->close();
        return -ENETUNREACH;
    }

    auto uploadOutput= uploadSocket->getOutputStream();
    auto uploadInput = uploadSocket->getInputStream();

    auto applyInfo = createApplyUploadMessage(file);
    int ret = uploadOutput->write(mConverter->generatePacket(applyInfo));
    ConfirmApplyUploadMessage confirmResp = waitResponse<ConfirmApplyUploadMessage>(uploadInput);

    if(confirmResp == nullptr) {
        uploadSocket->close();
        return  -ENETUNREACH;
    } else if(!confirmResp->isPermitted()) {
        uploadSocket->close();
        return -EEXIST;
    }

    if(confirmResp->data != nullptr && confirmResp->data->toString()->equals(st(ArchiveMessage)::kReject)) {
        uploadSocket->close();
        return -EACCES;
    }

    FileInputStream inputStream = createFileInputStream(file);
    inputStream->open();
    
    ByteArray data = createByteArray(32*1024);
    while(1) {
        int len = inputStream->read(data);
        if(len <= 0) {
            break;
        }

        data->quickShrink(len);
        if(uploadOutput->write(data) <= 0) {
            uploadOutput->close();
            return -ENETUNREACH;
        }
        data->quickRestore();
    }

    inputStream->close();
    return 0;
}

int _ArchiveConnection::connect() {
    mSock = createSocketBuilder()->setAddress(mAddress)->newSocket();
    Inspect(mSock->connect() < 0,-1);

    mInput = mSock->getInputStream();
    mOutput = mSock->getOutputStream();
    return 0;
}

int _ArchiveConnection::close() {
    if(mInput != nullptr) {
        mInput->close();
        mOutput->close();
        mInput = nullptr;
        mOutput = nullptr;
    }

    if(mSock != nullptr) {
        mSock->close();
        mSock = nullptr;
    }
    return 0;
}

}

