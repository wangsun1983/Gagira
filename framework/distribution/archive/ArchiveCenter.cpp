#include <algorithm>

#include "ArchiveCenter.hpp"
#include "FileInputStream.hpp"
#include "ForEveryOne.hpp"
#include "SocketBuilder.hpp"
#include "Inet4Address.hpp"
#include "Inet6Address.hpp"
#include "InetLocalAddress.hpp"
#include "ArchiveMessage.hpp"

namespace gagira {

//----ArchiveCenterUploadMonitor---
const int _ArchiveCenterUploadMonitor::kBusyLevel = 32;

_ArchiveCenterUploadRecord::_ArchiveCenterUploadRecord() {
    mParser = createDistributeMessageParser(1024*32);
}

_ArchiveCenterUploadMonitor::_ArchiveCenterUploadMonitor(ServerSocket socket,String savedPath,_ArchiveCenter *c) {
    mServer = socket;
    mMonitor = createSocketMonitor();
    mMonitor->bind(mServer,AutoClone(this));
    mRecords = createConcurrentHashMap<Socket,ArchiveCenterUploadRecord>();

    mConverter = createDistributeMessageConverter();
    mSavedPath = savedPath;
    mCenter = c;
}

void _ArchiveCenterUploadMonitor::onSocketMessage(st(Net)::Event event,Socket socket,ByteArray array) {
    switch(event) {
        case st(Net)::Event::Message: {
            auto record = mRecords->get(socket);
            if(record == nullptr) {
                record = createArchiveCenterUploadRecord();
                mRecords->put(socket,record);
                record->mStatus = WaitClientApplyInfo;
            }
            if(record->mStatus == WaitClientApplyInfo) {
                record->mParser->pushData(array);
                auto response = record->mParser->doParse();
                if(response != nullptr && response->size() != 0) {
                    ByteArray applyInfoData = response->get(0);
                    auto msg = mConverter->generateMessage<ApplyUploadMessage>(applyInfoData);
                    String path = mCenter->transformFilePath(createDistributeLinker(socket),msg);
                    ConfirmApplyUploadMessage response = nullptr;
                    if(path->equals(st(ArchiveMessage)::kReject)) {
                        response = createConfirmApplyUploadMessage(EPERM);
                        //response->data = st(ArchiveMessage)::kReject->toByteArray();
                        socket->getOutputStream()->write(mConverter->generatePacket(response));
                        break;
                    }

                    File file = createFile(path);
                    if(!file->exists()) {
                        file->createNewFile();
                        record->mPath = file->getAbsolutePath();
                        record->mFileSize = msg->getUploadFileLength();
                        record->mOutputStream = createFileOutputStream(file);
                        if(!record->mOutputStream->open()) {
                            //TODO
                        }
                        response = createConfirmApplyUploadMessage();
                        record->mStatus = WaitClientMessage;
                    } else {
                        response = createConfirmApplyUploadMessage(EEXIST);
                    }
                    socket->getOutputStream()->write(mConverter->generatePacket(response));
                }
            } else if(record->mStatus == WaitClientMessage) {
                int ret = record->mOutputStream->write(array);
                //TODO
                record->mFileSize -= array->size();
                if(record->mFileSize == 0) {
                    mRecords->remove(socket);
                    mMonitor->unbind(socket);
                    record->mOutputStream->close();
                }
            }
        } break;

        case st(Net)::Event::Disconnect: {
            auto record = mRecords->remove(socket);
            if(record->mOutputStream != nullptr) {
                record->mOutputStream->close();
            }
            mMonitor->unbind(socket);
        } break;
    }
}

uint32_t _ArchiveCenterUploadMonitor::getPort() {
    return mServer->getInetAddress()->getPort();
}

bool _ArchiveCenterUploadMonitor::isBusy() {
    return mRecords->size() > kBusyLevel;
}

//--- ArchiveCenter ---
_ArchiveCenter::_ArchiveCenter(String url,ArchiveOption option):_DistributeCenter(url,Cast<DistributeOption>(option)) {
    mConverter = createDistributeMessageConverter();
    mDownloadPool = createExecutorBuilder()->setMinThreadNum(2)->newCachedThreadPool();
    mSavedPath = option->getSavedPath();
    mThreads = createConcurrentQueue<ArchiveCenterUploadMonitor>();
    mDownloadRequests = createConcurrentHashMap<DistributeLinker,File>();
    mReadLinks = createConcurrentHashMap<DistributeLinker,FileInputStream>();
    mWriteLinks = createConcurrentHashMap<DistributeLinker,FileOutputStream>();

    mOption = option;
    mCurrentPort = createAtomicUint32(mOption->getMonitorStartPort());
    mHandler = mOption->getHandler();
    
}

_ArchiveCenter::~_ArchiveCenter() {

}

int _ArchiveCenter::onMessage(DistributeLinker linker,ByteArray data) {
    auto msg = mConverter->generateMessage<ArchiveMessage>(data);
    switch(msg->event) {
        case st(ArchiveMessage)::ApplyUploadConnect: {
            ArchiveCenterUploadMonitor selected = nullptr;

            ForEveryOne(m,mThreads) {
                if(!m->isBusy()) {
                    selected = m;
                }
            }

            if(selected == nullptr) {
                selected = createUploadMonitor();
                mThreads->add(selected);
            }
            ConfirmUploadConnectMessage response = createConfirmUploadConnectMessage(selected->getPort());
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(ArchiveMessage)::ApplyOpen: {
            String path = transformFilePath(linker,msg);
            if(path->equals(st(ArchiveMessage)::kReject)) {
                auto response = createConfirmReadMessage();
                response->data = st(ArchiveMessage)::kReject->toByteArray();
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                break;
            }
            
            uint64_t flags = msg->getFlags();
            FileInputStream input = nullptr;
            FileOutputStream output = nullptr;
            if(flags == O_RDONLY) {
                input = mReadLinks->get(linker);
                if(input != nullptr) {
                    input->close();
                    mReadLinks->remove(linker);
                }
            } else {
                output = mWriteLinks->get(linker);
                if(output != nullptr) {
                    output->close();
                    mWriteLinks->remove(linker);
                }
            }

            auto resp = createConfirmOpenMessage(createFile(path));
            if(resp->isPermitted()) {
                if(flags == O_RDONLY) {
                    input = createFileInputStream(path);
                    input->open();
                    mReadLinks->put(linker,input);
                } else {
                    output = createFileOutputStream(path);
                    if(flags & O_APPEND) {
                        output->open(st(IO)::FileControlFlags::Append);
                    } else {
                        output->open(st(IO)::FileControlFlags::Trunc);
                    }
                    mWriteLinks->put(linker,output);
                }
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(resp));
        } break;

        case st(ArchiveMessage)::ApplySeekTo: {
            auto input = mReadLinks->get(linker);
            int ret = -1;
            if(input != nullptr) {
                ret = input->seekTo(msg->getStartPos());
            }

            auto response = createConfirmSeekToMessage(ret > 0?0:EIO);
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(ArchiveMessage)::ProcessRead: {
            auto inputStream = mReadLinks->get(linker);
            uint64_t length = msg->getReadLength();
            if(inputStream == nullptr) {
                auto resp = createConfirmReadMessage();
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(resp));
            } else {
                mDownloadPool->submit([&](DistributeLinker mylinker,FileInputStream input,uint64_t length){
                    ByteArray data = createByteArray(length);
                    int ret = input->read(data);
                    auto resp = createConfirmReadMessage(data);
                    mylinker->getSocket()->getOutputStream()->write(mConverter->generatePacket(resp));
                },linker,inputStream,length);
            }
        } break;

        case st(ArchiveMessage)::ApplyDel: {
            String path = transformFilePath(linker,msg);
            ConfirmDelMessage response = nullptr;
            if(path->equals(st(ArchiveMessage)::kReject)) {
                response = createConfirmDelMessage(EPERM);
            } else {
                File file = createFile(path);
                if(!file->exists()) {
                    response = createConfirmDelMessage(ENOENT);
                } else {
                    file->removeAll();
                    response = createConfirmDelMessage();
                }
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(ArchiveMessage)::ApplyRename: {
            String path = transformFilePath(linker,msg);
            ConfirmRenameMessage response = nullptr;
            if(path->equals(st(ArchiveMessage)::kReject)) {
                response = createConfirmRenameMessage(EPERM);
            } else {
                File file = createFile(path);
                if(!file->exists()) {
                    response = createConfirmRenameMessage(ENOENT);
                } else {
                    String newname = msg->data->toString();
                    response = createConfirmRenameMessage(file->rename(newname));
                }
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(ArchiveMessage)::ApplyWrite: {
            auto outputStream = mWriteLinks->get(linker);
            ConfirmWriteMessage resp = nullptr;
            if(outputStream != nullptr) {
                outputStream->write(msg->data);
                resp = createConfirmWriteMessage();
            } else {
                resp = createConfirmWriteMessage(EPIPE);
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(resp));
        } break;

        case st(ArchiveMessage)::ApplyDownload: {
            String path = transformFilePath(linker,msg);//getDownloadFilePath(msg->filename);
            if(path->equals(st(ArchiveMessage)::kReject)) {
                auto response = createConfirmDownloadMessage();
                response->data = st(ArchiveMessage)::kReject->toByteArray();
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                break;
            }

            File file = createFile(path);
            ConfirmDownloadMessage msg = nullptr;
            if(file->exists()) {
                mDownloadRequests->put(linker,file);
                msg = createConfirmDownloadMessage(file);
            } else {
                msg = createConfirmDownloadMessage();
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(msg));
        } break;

        case st(ArchiveMessage)::ProcessDownload: {
            mDownloadPool->submit([&](DistributeLinker mylinker){
                ByteArray buff = createByteArray(32*1024);
                File file = mDownloadRequests->remove(mylinker);
                FileInputStream stream = createFileInputStream(file);
                stream->open();
                OutputStream output = mylinker->getSocket()->getOutputStream();
                while(1) {
                    int ret = stream->read(buff);
                    if(ret <= 0) {
                        break;
                    }
                    buff->quickShrink(ret);
                    output->write(buff);
                    buff->quickRestore();
                }
                stream->close();
            },linker);
        } break;

        case st(ArchiveMessage)::QueryInfo: {
            String path = transformFilePath(linker,msg);

            auto msg = createConfirmQueryInfoMessage();
            if(!path->equals(st(ArchiveMessage)::kReject)) {
                File file = createFile(path);
                msg = createConfirmQueryInfoMessage(file);
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(msg));
        } break;
    }

    return 0;
}

int _ArchiveCenter::onNewClient(DistributeLinker linker) {
    return 0;
}

int _ArchiveCenter::onDisconnectClient(DistributeLinker linker) {
    auto input = mReadLinks->remove(linker);
    if(input != nullptr) {
        input->close();
    }

    auto output = mWriteLinks->remove(linker);
    if(output != nullptr) {
        output->close();
    }
    return 0;
}

ArchiveCenterUploadMonitor _ArchiveCenter::createUploadMonitor() {
    while(1) {
        uint32_t port = mCurrentPort->getAndIncrement();
        if(port > mOption->getMonitorEndPort()) {
            mCurrentPort->set(mOption->getMonitorStartPort());
            return nullptr;
        }
        String addrStr = mAddress->getAddress();
        InetAddress addr = nullptr;
        switch(mAddress->getFamily()) {
            case st(Net)::Family::Ipv4: {
                addr = createInet4Address(addrStr,port);
            } break;

            case st(Net)::Family::Ipv6: {
                addr = createInet6Address(addrStr,port);
            } break;

            case st(Net)::Family::Local: {
                //TODO
            } break;
        }
        
        ServerSocket sock = createSocketBuilder()->setAddress(addr)->newServerSocket();
        if(sock->bind() < 0) {
            continue;
        }

        return createArchiveCenterUploadMonitor(sock,mOption->getSavedPath(),this);
    }

    return nullptr;
}

String _ArchiveCenter::transformFilePath(DistributeLinker link,ArchiveMessage msg) {
    String path = nullptr;
    if(mHandler != nullptr) {
        ArchiveHandleResult ret = mHandler->onRequest(link,msg);
        if(ret != nullptr) {
            path = ret->path;
        }
    }

    if(path == nullptr) {
        path = mSavedPath->append("/",msg->filename);
    }
    return path;
}

}
