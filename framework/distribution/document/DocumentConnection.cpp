#include "DocumentConnection.hpp"
#include "Inet4Address.hpp"
#include "Inet6Address.hpp"
#include "SocketBuilder.hpp"
#include "FileInputStream.hpp"
#include "HttpUrl.hpp"
#include "InitializeException.hpp"
#include "DocumentMessage.hpp"
#include "FileOutputStream.hpp"

using namespace obotcha;

namespace gagira {

_DocumentConnection::_DocumentConnection(String s) {
    HttpUrl url = createHttpUrl(s);
    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find DocumentCenter");
    }
    mConverter = createDistributeMessageConverter();
}

_DocumentConnection::~_DocumentConnection() {
    close();
}

int _DocumentConnection::download(String filename,String savepath) {
    ApplyDownloadMessage msg = createApplyDownloadMessage(filename);
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -ENETUNREACH;
    }

    ConfirmDownloadMessage resp = waitResponse<ConfirmDownloadMessage>(mInput);
    if(resp->data != nullptr && resp->data->toString()->equals(st(DocumentMessage)::kReject)) {
        return -EACCES;
    }

    if(resp->length == 0) {
        return -ENOENT;
    }

    uint64_t filesize = resp->length;
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

int _DocumentConnection::upload(File file) {
    auto msg = createApplyUploadMessage();
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -ENETUNREACH;
    }

    ConfirmUploadMessage resp = waitResponse<ConfirmUploadMessage>(mInput);
    int port = resp->port;
    InetAddress uploadAddress = nullptr;
    switch(mAddress->getFamily()) {
        case st(InetAddress)::IPV4:
            uploadAddress = createInet4Address(mAddress->getAddress(),port);
        break;

        case st(InetAddress)::IPV6:
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

    auto applyInfo = createApplyUploadInfoMessage(file);
    uploadOutput->write(mConverter->generatePacket(applyInfo));
    ConfirmApplyUploadInfoMessage confirmResp = waitResponse<ConfirmApplyUploadInfoMessage>(uploadInput);
    if(confirmResp == nullptr) {
        uploadSocket->close();
        return  -ENETUNREACH;
    } else if(confirmResp->length != 0) {
        uploadSocket->close();
        return -EEXIST;
    }

    if(confirmResp->data != nullptr && confirmResp->data->toString()->equals(st(DocumentMessage)::kReject)) {
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

int _DocumentConnection::connect() {
    mSock = createSocketBuilder()->setAddress(mAddress)->newSocket();
    Inspect(mSock->connect() < 0,-1);

    mInput = mSock->getInputStream();
    mOutput = mSock->getOutputStream();
    return 0;
}

int _DocumentConnection::close() {
    if(mSock != nullptr) {
        mInput->close();
        mOutput->close();
        mSock->close();
        mSock = nullptr;
    }
    return 0;
}

}

