#include "ArchiveMessage.hpp"
#include "Md.hpp"

using namespace obotcha;

namespace gagira {

//---_ArchiveMessage---
_ArchiveMessage::_ArchiveMessage() {
    permitFlag = 0;
}

int _ArchiveMessage::getEvent() {
    return event;
}
uint64_t _ArchiveMessage::getPort() {
    return privateData;
}

uint64_t _ArchiveMessage::getUploadFileLength() {
    return privateData;
}

uint64_t _ArchiveMessage::getDownloadFileSize() {
    return privateData2;
}

uint64_t _ArchiveMessage::getQueryFileSize() {
    return privateData;
}

bool _ArchiveMessage::isPermitted() {
    printf("permitFlag is %d \n",permitFlag);
    return this->permitFlag == 0;
}

uint32_t _ArchiveMessage::getStartPos() {
    return (this->privateData2 & 0xFFFF);
}

uint64_t _ArchiveMessage::getReadLength() {
    return this->privateData2;
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

uint32_t _ArchiveMessage::getPermitFlag() {
    return this->permitFlag;
}

uint64_t _ArchiveMessage::getFileNo() {
    return this->privateData;
}

uint64_t _ArchiveMessage::getSeekType() {
    return (this->privateData2 & 0xFFFF0000)>>31;
}

String _ArchiveMessage::getFileName() {
    return this->filename;
}

ByteArray _ArchiveMessage::getData() {
    return this->data;
}

ByteArray _ArchiveMessage::getVerifyData() {
    return this->verifyData;
}

String _ArchiveMessage::getRenameOriginalName() {
    return this->filename;
}

String _ArchiveMessage::getRenameNewName() {
    return this->data->toString();
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

_ConfirmUploadConnectMessage::_ConfirmUploadConnectMessage(uint32_t port) {
    this->event = ConfirmUploadConnect;
    this->privateData = port;
}

//ApplyUploadInfoMessage(File);
_ApplyUploadMessage::_ApplyUploadMessage() {
    this->event = ApplyUpload; 
}

_ApplyUploadMessage::_ApplyUploadMessage(File file) {
    this->event = ApplyUpload;
    this->filename = file->getName();
    this->privateData = file->length();
    //we should compute md5sum value
    Md md5sum = Md::New();
    this->verifyData = md5sum->encodeFile(file)->toByteArray();
}

//--- ConfirmApplyUploadInfoMessage ---
_ConfirmApplyUploadMessage::_ConfirmApplyUploadMessage() {
    this->event = ConfirmUpload;
}

_ConfirmApplyUploadMessage::_ConfirmApplyUploadMessage(uint32_t result) {
    this->event = ConfirmUpload;
    this->permitFlag = result;
}

_CompleteUploadMessage::_CompleteUploadMessage() {
    this->event = CompleteUpload;
}

_CompleteUploadMessage::_CompleteUploadMessage(uint32_t result) {
    this->event = CompleteUpload;
    this->permitFlag = result;
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
    //do nothing
}

_ConfirmDownloadMessage::_ConfirmDownloadMessage(uint32_t result) {
    this->event = ConfirmDownload;
    this->permitFlag = result;
}

_ConfirmDownloadMessage::_ConfirmDownloadMessage(File file,uint64_t id) {
    this->event = ConfirmDownload;
    this->filename = file->getName();
    this->privateData = id;
    this->privateData2 = file->length();
    Md md5sum = Md::New();
    this->verifyData = md5sum->encodeFile(file)->toByteArray();
    printf("permit flag is %d \n",permitFlag);
}

_ProcessDownloadMessage::_ProcessDownloadMessage(uint64_t fileno) {
    this->event = ProcessDownload;
    this->privateData = fileno;
}

_QueryInfoMessage::_QueryInfoMessage(String filename) {
    this->event = QueryInfo;
    this->filename = filename;
}

_ConfirmQueryInfoMessage::_ConfirmQueryInfoMessage() {
    this->event = ConfirmQueryInfo;
}

_ConfirmQueryInfoMessage::_ConfirmQueryInfoMessage(uint32_t result) {
    this->event = ConfirmQueryInfo;
    this->permitFlag = result;
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

_ConfirmOpenMessage::_ConfirmOpenMessage(uint64_t fileno,uint32_t result) {
    this->event = ConfirmOpen;
    this->permitFlag = result;
    this->privateData = fileno;
}

_ConfirmOpenMessage::_ConfirmOpenMessage() {
    this->event = ConfirmOpen;
}

_ApplySeekToMessage::_ApplySeekToMessage(uint64_t fileno,uint32_t start,Type type) {
    this->event = ApplySeekTo;
    this->privateData = fileno;
    this->privateData2 = start;
    this->privateData2 |= (((uint32_t)type)<<31);
}

_ConfirmSeekToMessage::_ConfirmSeekToMessage(uint32_t result) {
    this->event = ConfirmSeekTo;
    this->permitFlag = result;
}

_ConfirmSeekToMessage::_ConfirmSeekToMessage() {
    this->event = ConfirmSeekTo;
}

_ApplyReadMessage::_ApplyReadMessage(uint64_t fileno,uint64_t len) {
    this->event = ProcessRead;
    this->privateData = fileno;
    this->privateData2 = len;
}

_ConfirmReadMessage::_ConfirmReadMessage() {
    this->event = ConfirmRead;
}

_ConfirmReadMessage::_ConfirmReadMessage(ByteArray data) {
    this->event = ConfirmRead;
    this->data = data;
    this->privateData2 = data->size();
}

_ConfirmReadMessage::_ConfirmReadMessage(uint64_t result) {
    this->event = ConfirmRead;
    this->permitFlag = result;
}

_ApplyWriteMessage::_ApplyWriteMessage(uint64_t fileno,ByteArray data) {
    this->event = ApplyWrite;
    this->data = data;
    this->privateData = fileno;
}

_ConfirmWriteMessage::_ConfirmWriteMessage() {
    this->event = ConfirmWrite;
}

_ConfirmWriteMessage::_ConfirmWriteMessage(uint64_t result) {
    this->event = ConfirmWrite;
    this->permitFlag = result;
}

_ApplyDelMessage::_ApplyDelMessage(String filename) {
    this->event = ApplyDel;
    this->filename = filename;
}

_ConfirmDelMessage::_ConfirmDelMessage() {
    this->event = ConfirmDel;
}

_ConfirmDelMessage::_ConfirmDelMessage(uint64_t result) {
    this->event = ConfirmDel;
    this->permitFlag = result;
}

_ApplyRenameMessage::_ApplyRenameMessage(String originalname,String newname) {
    this->event = ApplyRename;
    this->filename = originalname;
    this->data = newname->toByteArray();
}

_ConfirmRenameMessage::_ConfirmRenameMessage() {
    this->event = ConfirmRename;
}

_ConfirmRenameMessage::_ConfirmRenameMessage(uint32_t result) {
    this->event = ConfirmRename;
    this->permitFlag = result;
}

_ApplyCloseStreamMessage::_ApplyCloseStreamMessage(uint64_t fileno) {
    this->event = ApplyCloseStream;
    this->privateData = fileno;
}


_ConfirmCloseStreamMessage::_ConfirmCloseStreamMessage() {
    this->event = ConfirmCloseStream;
}

_ConfirmCloseStreamMessage::_ConfirmCloseStreamMessage(uint32_t result) {
    this->event = ConfirmCloseStream;
    this->permitFlag = result;
}

}

