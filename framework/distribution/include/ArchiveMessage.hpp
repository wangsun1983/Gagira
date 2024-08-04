#ifndef __GAGRIA_ARCHIVE_MESSAGE_HPP__
#define __GAGRIA_ARCHIVE_MESSAGE_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "SocketMonitor.hpp"
#include "SpaceOption.hpp"
#include "InetAddress.hpp"
#include "ServerSocket.hpp"
#include "DistributeCenter.hpp"
#include "DistributeOption.hpp"
#include "DistributeMessageConverter.hpp"
#include "File.hpp"
#include "Serializable.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ArchiveMessage) IMPLEMENTS(Serializable) {
public:
    static const String kReject;
    enum Type {
        ApplyUploadConnect = 0,
        ConfirmUploadConnect,
        ApplyUpload,
        ConfirmUpload,
        CompleteUpload,
        ProcessUpload,
        QueryInfo,
        ConfirmQueryInfo,

        ApplyDownload,
        ConfirmDownload,
        ProcessDownload,
        ApplyOpen,
        ConfirmOpen,
        ProcessRead,
        ConfirmRead,
        ApplySeekTo,
        ConfirmSeekTo,
        ApplyWrite,
        ConfirmWrite,

        ApplyDel,
        ConfirmDel,

        ApplyRename,
        ConfirmRename,

        ApplyCloseStream,
        ConfirmCloseStream,
    };

    _ArchiveMessage();
    int getEvent();
    uint64_t getPort();
    uint64_t getUploadFileLength();
    uint64_t getDownloadFileSize();
    uint64_t getQueryFileSize();
    bool isPermitted();
    uint32_t getStartPos();
    uint64_t getReadLength();
    uint64_t getDataSize();
    uint64_t getWriteLength();
    uint64_t getFlags();
    uint32_t getPermitFlag();
    uint64_t getFileNo();
    uint64_t getSeekType(); //read or write
    String getFileName();
    String getRenameOriginalName();
    String getRenameNewName();
    ByteArray getData();
    ByteArray getVerifyData();

protected:
    int event;
    String filename;
    ByteArray data;
    ByteArray verifyData; //save crc/md5 checkdata?
    uint64_t privateData;
    uint64_t privateData2;
    uint32_t permitFlag;

    DECLARE_REFLECT_FIELD(ArchiveMessage,event,filename,privateData,privateData2,data,permitFlag,verifyData);
};

DECLARE_CLASS(ApplyUploadConnectMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyUploadConnectMessage();
};

DECLARE_CLASS(ConfirmUploadConnectMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmUploadConnectMessage();
    _ConfirmUploadConnectMessage(uint32_t port);
};

DECLARE_CLASS(ApplyUploadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyUploadMessage();
    _ApplyUploadMessage(File);
};

DECLARE_CLASS(ConfirmApplyUploadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmApplyUploadMessage();
    _ConfirmApplyUploadMessage(uint32_t result);
};

DECLARE_CLASS(CompleteUploadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _CompleteUploadMessage();
    _CompleteUploadMessage(uint32_t result);
};

DECLARE_CLASS(ApplyDownloadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyDownloadMessage();
    _ApplyDownloadMessage(String filename);
};

DECLARE_CLASS(ConfirmDownloadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmDownloadMessage();
    _ConfirmDownloadMessage(File,uint64_t id);   
    _ConfirmDownloadMessage(uint32_t result);
};

DECLARE_CLASS(ProcessDownloadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ProcessDownloadMessage(uint64_t);   
};

DECLARE_CLASS(QueryInfoMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _QueryInfoMessage(String);
};

DECLARE_CLASS(ConfirmQueryInfoMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmQueryInfoMessage();
    _ConfirmQueryInfoMessage(File);
    _ConfirmQueryInfoMessage(uint32_t result);
};

DECLARE_CLASS(ApplyOpenMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyOpenMessage(String filename,uint64_t flags = O_RDONLY);
};

DECLARE_CLASS(ConfirmOpenMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmOpenMessage();
    _ConfirmOpenMessage(uint64_t fileno,uint32_t result);
    //_ConfirmOpenMessage(uint64_t result);
};

DECLARE_CLASS(ApplySeekToMessage) IMPLEMENTS(ArchiveMessage) {
public:
    enum Type {
        Read = 0,
        Write
    };
    _ApplySeekToMessage(uint64_t fileno,uint32_t start,Type);
};

DECLARE_CLASS(ConfirmSeekToMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmSeekToMessage();
    _ConfirmSeekToMessage(uint32_t result);
};

DECLARE_CLASS(ApplyReadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyReadMessage(uint64_t fileno,uint64_t readlen);
};

DECLARE_CLASS(ConfirmReadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmReadMessage();
    _ConfirmReadMessage(ByteArray);
    _ConfirmReadMessage(uint64_t result);
};

DECLARE_CLASS(ApplyWriteMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyWriteMessage(uint64_t fileno,ByteArray);
};

DECLARE_CLASS(ConfirmWriteMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmWriteMessage();
    _ConfirmWriteMessage(uint64_t result);
};

DECLARE_CLASS(ApplyDelMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyDelMessage(String);
};

DECLARE_CLASS(ConfirmDelMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmDelMessage();
    _ConfirmDelMessage(uint64_t result);
};

DECLARE_CLASS(ApplyRenameMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyRenameMessage(String originalname,String newname);
};

DECLARE_CLASS(ConfirmRenameMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmRenameMessage();
    _ConfirmRenameMessage(uint32_t result);
};


DECLARE_CLASS(ApplyCloseStreamMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyCloseStreamMessage(uint64_t fileno);
};

DECLARE_CLASS(ConfirmCloseStreamMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmCloseStreamMessage();
    _ConfirmCloseStreamMessage(uint32_t result);
};

}
#endif
