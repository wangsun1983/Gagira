#ifndef __GAGRIA_DOCUMENT_MESSAGE_HPP__
#define __GAGRIA_DOCUMENT_MESSAGE_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "SocketMonitor.hpp"
#include "SpaceOption.hpp"
#include "InetAddress.hpp"
#include "ServerSocket.hpp"
#include "DistributeCenter.hpp"
#include "DistributeOption.hpp"
#include "DistributeMessageConverter.hpp"
#include "Serializable.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DocumentMessage) IMPLEMENTS(Serializable) {
public:
    static const String kReject;
    enum Type {
        ApplyUpload,
        ConfirmUpload,
        ApplyUploadInfo,
        ConfirmUploadInfo,
        ProcessUpload,

        ApplyDownload,
        ConfirmDownload,
        ProcessDownload
    };

    int event;
    String filename;
    uint64_t length;
    ByteArray data;
    uint32_t port;
    
    _DocumentMessage();
    DECLARE_REFLECT_FIELD(DocumentMessage,event,filename,length,data,port);
};

DECLARE_CLASS(ApplyUploadMessage) IMPLEMENTS(DocumentMessage) {
public:
    _ApplyUploadMessage();
};

DECLARE_CLASS(ConfirmUploadMessage) IMPLEMENTS(DocumentMessage) {
public:
    _ConfirmUploadMessage();
    _ConfirmUploadMessage(uint32_t);
};

DECLARE_CLASS(ApplyUploadInfoMessage) IMPLEMENTS(DocumentMessage) {
public:
    _ApplyUploadInfoMessage();
    _ApplyUploadInfoMessage(File);
};

DECLARE_CLASS(ConfirmApplyUploadInfoMessage) IMPLEMENTS(DocumentMessage) {
public:
    _ConfirmApplyUploadInfoMessage();
};

DECLARE_CLASS(ApplyDownloadMessage) IMPLEMENTS(DocumentMessage) {
public:
    _ApplyDownloadMessage();
    _ApplyDownloadMessage(String);
};

DECLARE_CLASS(ConfirmDownloadMessage) IMPLEMENTS(DocumentMessage) {
public:
    _ConfirmDownloadMessage();
    _ConfirmDownloadMessage(File);   
};

DECLARE_CLASS(ProcessDownloadMessage) IMPLEMENTS(DocumentMessage) {
public:
    _ProcessDownloadMessage();   
};

}
#endif
