#include "DocumentMessage.hpp"

using namespace obotcha;

namespace gagira {

const String _DocumentMessage::kReject = createString("?<Reject>?");
   

//---_DocumentMessage---
_DocumentMessage::_DocumentMessage() {
    //nothing;
}

//--- ApplyUploadMessage ---
_ApplyUploadMessage::_ApplyUploadMessage() {
    this->event = ApplyUpload;
}

//--- ConfirmUploadMessage ---
_ConfirmUploadMessage::_ConfirmUploadMessage() {
    //nothing
    this->event = ConfirmUpload;
}

_ConfirmUploadMessage::_ConfirmUploadMessage(uint32_t v) {
    this->event = ConfirmUpload;
    this->port = v;
}

//ApplyUploadInfoMessage(File);
_ApplyUploadInfoMessage::_ApplyUploadInfoMessage() {
    
}

_ApplyUploadInfoMessage::_ApplyUploadInfoMessage(File file) {
    this->event = ApplyUploadInfo;
    this->filename = file->getName();
    this->length = file->length();
}

//--- ConfirmApplyUploadInfoMessage ---
_ConfirmApplyUploadInfoMessage::_ConfirmApplyUploadInfoMessage() {
    this->event = ConfirmUploadInfo;
    this->length = 0;
}

//--- ApplyDownloadMessage ---
_ApplyDownloadMessage::_ApplyDownloadMessage() {
    this->event = ApplyDownload;
    //do nothing
}

_ApplyDownloadMessage::_ApplyDownloadMessage(String filename) {
    this->event = ApplyDownload;
    this->filename = filename;
}

//--- ConfirmDownloadMessage ---
_ConfirmDownloadMessage::_ConfirmDownloadMessage() {
    this->event = ConfirmDownload;
    this->length = 0;
    //do nothing
}

_ConfirmDownloadMessage::_ConfirmDownloadMessage(File file) {
    this->event = ConfirmDownload;
    this->filename = file->getName();
    this->length = file->length(); 
}

_ProcessDownloadMessage::_ProcessDownloadMessage() {
    this->event = ProcessDownload;
}

}

