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

        ApplyDel,   //TODO
        ConfirmDel,
    };

    int event;
    String filename;
    ByteArray data;
    uint32_t port;
    // union {
    //     uint64_t length;
    //     uint64_t result; 0:success; >0:fail reason
    //     uint64_t flags;
    //     uint64_t readfrom;
    //     uint64_t readlen;
    // } privateData;
    uint64_t privateData;

    uint64_t getUploadFileLength();
    uint64_t getDownloadFileSize();
    uint64_t getQueryFileSize();
    bool isPermitted();
    uint64_t getStartPos();
    uint64_t getReadLength();
    uint64_t getDataSize();
    uint64_t getWriteLength();
    uint64_t getFlags();
    
    _ArchiveMessage();
    DECLARE_REFLECT_FIELD(ArchiveMessage,event,filename,privateData,data,port);
};

DECLARE_CLASS(ApplyUploadConnectMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyUploadConnectMessage();
};

DECLARE_CLASS(ConfirmUploadConnectMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmUploadConnectMessage();
    _ConfirmUploadConnectMessage(uint32_t);
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

DECLARE_CLASS(ApplyDownloadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyDownloadMessage();
    _ApplyDownloadMessage(String);
};

DECLARE_CLASS(ConfirmDownloadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmDownloadMessage();
    _ConfirmDownloadMessage(File);   
};

DECLARE_CLASS(ProcessDownloadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ProcessDownloadMessage();   
};

DECLARE_CLASS(QueryInfoMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _QueryInfoMessage(String);
};

DECLARE_CLASS(ConfirmQueryInfoMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmQueryInfoMessage();
    _ConfirmQueryInfoMessage(File);
};

DECLARE_CLASS(ApplyOpenMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyOpenMessage(String filename,uint64_t flags = O_RDONLY);
};

DECLARE_CLASS(ConfirmOpenMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmOpenMessage();
    _ConfirmOpenMessage(File);
};

DECLARE_CLASS(ApplySeekToMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplySeekToMessage(uint32_t start);
};

DECLARE_CLASS(ConfirmSeekToMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmSeekToMessage();
    _ConfirmSeekToMessage(uint32_t result);
};

DECLARE_CLASS(ApplyReadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyReadMessage(uint64_t readlen);
};

DECLARE_CLASS(ConfirmReadMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ConfirmReadMessage();
    _ConfirmReadMessage(ByteArray);
};

DECLARE_CLASS(ApplyWriteMessage) IMPLEMENTS(ArchiveMessage) {
public:
    _ApplyWriteMessage(ByteArray);
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

}
#endif
