#include "ArchiveMessage.hpp"

using namespace obotcha;

namespace gagira {

const String _ArchiveMessage::kReject = createString("?<Reject>?");
   

//---_ArchiveMessage---
_ArchiveMessage::_ArchiveMessage() {
    //nothing;
}

uint64_t _ArchiveMessage::getUploadFileLength() {
    return privateData;
}

uint64_t _ArchiveMessage::getDownloadFileSize() {
    return privateData;
}

uint64_t _ArchiveMessage::getQueryFileSize() {
    return privateData;
}

bool _ArchiveMessage::isPermitted() {
    return this->privateData == 0;
}

uint64_t _ArchiveMessage::getStartPos() {
    return this->privateData;
}

uint64_t _ArchiveMessage::getReadLength() {
    return this->privateData;
}

uint64_t _ArchiveMessage::getDataSize() {
    return this->privateData;
}

uint64_t _ArchiveMessage::getWriteLength() {
    return this->privateData;
}

uint64_t _ArchiveMessage::getFlags() {
    return this->privateData;
}

//--- ApplyUploadMessage ---
_ApplyUploadConnectMessage::_ApplyUploadConnectMessage() {
    this->event = ApplyUploadConnect;
}

//--- ConfirmUploadMessage ---
_ConfirmUploadConnectMessage::_ConfirmUploadConnectMessage() {
    //nothing
    this->event = ConfirmUploadConnect;
}

_ConfirmUploadConnectMessage::_ConfirmUploadConnectMessage(uint32_t v) {
    this->event = ConfirmUploadConnect;
    this->port = v;
}

//ApplyUploadInfoMessage(File);
_ApplyUploadMessage::_ApplyUploadMessage() {
    
}

_ApplyUploadMessage::_ApplyUploadMessage(File file) {
    this->event = ApplyUpload;
    this->filename = file->getName();
    this->privateData = file->length();
}

//--- ConfirmApplyUploadInfoMessage ---
_ConfirmApplyUploadMessage::_ConfirmApplyUploadMessage() {
    this->event = ConfirmUpload;
    this->privateData = 0;
}

_ConfirmApplyUploadMessage::_ConfirmApplyUploadMessage(uint32_t result) {
    this->event = ConfirmUpload;
    this->privateData = result;
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
    this->privateData = 0;
    //do nothing
}

_ConfirmDownloadMessage::_ConfirmDownloadMessage(File file) {
    this->event = ConfirmDownload;
    this->filename = file->getName();
    this->privateData = EEXIST; 
}

_ProcessDownloadMessage::_ProcessDownloadMessage() {
    this->event = ProcessDownload;
}

_QueryInfoMessage::_QueryInfoMessage(String filename) {
    this->event = QueryInfo;
    this->filename = filename;
}

_ConfirmQueryInfoMessage::_ConfirmQueryInfoMessage() {
    this->event = ConfirmQueryInfo;
    this->privateData = 0;
}

_ConfirmQueryInfoMessage::_ConfirmQueryInfoMessage(File file) {
    this->event = ConfirmQueryInfo;
    this->privateData = file->exists()?file->length():0;
}

_ApplyOpenMessage::_ApplyOpenMessage(String filename,uint64_t flags) {
    this->event = ApplyOpen;
    this->filename = filename;
    this->privateData = flags;
}

_ConfirmOpenMessage::_ConfirmOpenMessage(File file) {
    this->event = ConfirmOpen;
    this->privateData = file->exists()?0:ENOENT;
}

_ConfirmOpenMessage::_ConfirmOpenMessage() {
    this->event = ConfirmOpen;
    this->privateData = ENOENT;
}

_ApplySeekToMessage::_ApplySeekToMessage(uint32_t start) {
    this->event = ApplySeekTo;
    this->privateData = start;
}

_ConfirmSeekToMessage::_ConfirmSeekToMessage(uint32_t result) {
    this->event = ConfirmSeekTo;
    this->privateData = result;
}

_ConfirmSeekToMessage::_ConfirmSeekToMessage() {
    this->event = ConfirmSeekTo;
    this->privateData = EIO;
}

_ApplyReadMessage::_ApplyReadMessage(uint64_t len) {
    this->event = ProcessRead;
    this->privateData = len;
}

_ConfirmReadMessage::_ConfirmReadMessage() {
    this->event = ConfirmRead;
    this->privateData = EIO;
}

_ConfirmReadMessage::_ConfirmReadMessage(ByteArray data) {
    this->event = ConfirmRead;
    this->data = data;
    this->privateData = data->size();
}

_ApplyWriteMessage::_ApplyWriteMessage(ByteArray data) {
    this->event = ApplyWrite;
    this->data = data;
}

_ConfirmWriteMessage::_ConfirmWriteMessage() {
    this->event = ConfirmWrite;
    this->privateData = 0;
}

_ConfirmWriteMessage::_ConfirmWriteMessage(uint64_t result) {
    this->event = ConfirmWrite;
    this->privateData = result;
}

_ApplyDelMessage::_ApplyDelMessage(String filename) {
    this->event = ApplyDel;
    this->filename = filename;
}

_ConfirmDelMessage::_ConfirmDelMessage() {
    this->event = ConfirmDel;
    this->privateData = 0;
}

_ConfirmDelMessage::_ConfirmDelMessage(uint64_t result) {
    this->event = ConfirmDel;
    this->privateData = result;
}

_ApplyRenameMessage::_ApplyRenameMessage(String originalname,String newname) {
    this->event = ApplyRename;
    this->filename = originalname;
    this->data = newname->toByteArray();
}

_ConfirmRenameMessage::_ConfirmRenameMessage() {
    this->event = ConfirmRename;
    this->privateData = 0;
}

_ConfirmRenameMessage::_ConfirmRenameMessage(uint64_t result) {
    this->event = ConfirmRename;
    this->privateData = result;
}


}

