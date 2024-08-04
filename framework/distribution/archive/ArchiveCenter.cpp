#include <algorithm>

#include "ArchiveCenter.hpp"
#include "FileInputStream.hpp"
#include "ForEveryOne.hpp"
#include "SocketBuilder.hpp"
#include "Inet4Address.hpp"
#include "Inet6Address.hpp"
#include "InetLocalAddress.hpp"
#include "ArchiveMessage.hpp"
#include "ArchiveHandleResult.hpp"
#include "Md.hpp"

namespace gagira {

//----ArchiveCenterUploadMonitor---
const int _ArchiveCenterUploadMonitor::kBusyLevel = 32;

_ArchiveCenterUploadRecord::_ArchiveCenterUploadRecord() {
    mParser = DistributeMessageParser::New(1024*32);
    mFileSize = 0;
}

_ArchiveCenterUploadMonitor::_ArchiveCenterUploadMonitor(ServerSocket socket,String savedPath,_ArchiveCenter *c) {
    mServer = socket;
    mMonitor = SocketMonitor::New();
    mMonitor->bind(mServer,AutoClone(this));
    mRecords = ConcurrentHashMap<Socket,ArchiveCenterUploadRecord>::New();

    mConverter = DistributeMessageConverter::New();
    mSavedPath = savedPath;
    mCenter = c;
}

void _ArchiveCenterUploadMonitor::onSocketMessage(st(Net)::Event event,Socket socket,ByteArray array) {
    switch(event) {
        case st(Net)::Event::Message: {
            auto record = mRecords->get(socket);
            if(record == nullptr) {
                record = ArchiveCenterUploadRecord::New();
                mRecords->put(socket,record);
                record->mStatus = WaitClientApplyInfo;
            }
            if(record->mStatus == WaitClientApplyInfo) {
                record->mParser->pushData(array);
                auto response = record->mParser->doParse();
                if(response != nullptr && response->size() != 0) {
                    ByteArray applyInfoData = response->get(0);
                    auto msg = mConverter->generateMessage<ApplyUploadMessage>(applyInfoData);
                    FetchRet(ret,path) = mCenter->transformFilePath(DistributeLinker::New(socket),msg);
                    ConfirmApplyUploadMessage response = nullptr;
                    if(ret == st(ArchiveHandleResult)::Reject) {
                        response = ConfirmApplyUploadMessage::New(EPERM);
                        socket->getOutputStream()->write(mConverter->generatePacket(response));
                        socket->getOutputStream()->flush();
                        mRecords->remove(socket);
                        mMonitor->unbind(socket);
                        if(record->mOutputStream != nullptr) {
                            record->mOutputStream->close();
                        }
                        break;
                    }
                    File file = File::New(path);
                    if(!file->exists()) {
                        file->createNewFile();
                        record->mPath = file->getAbsolutePath();
                        record->mFileSize = msg->getUploadFileLength();
                        record->mOutputStream = FileOutputStream::New(file);
                        if(!record->mOutputStream->open()) {
                            //TODO
                        }
                        response = ConfirmApplyUploadMessage::New();
                        record->mStatus = WaitClientMessage;
                        record->mVerifyData = msg->getVerifyData();
                    } else {
                        response = ConfirmApplyUploadMessage::New(EEXIST);
                    }
                    socket->getOutputStream()->write(mConverter->generatePacket(response));
                }
            } else if(record->mStatus == WaitClientMessage) {
                int ret = record->mOutputStream->write(array);
                record->mFileSize -= array->size();
                if(record->mFileSize == 0) {
                    //mRecords->remove(socket);
                    //mMonitor->unbind(socket);
                    //check current file md5sum
                    Md md5sum = Md::New();
                    auto mdValue = md5sum->encodeFile(File::New(record->mPath));
                    CompleteUploadMessage response = nullptr;
                    if(mdValue->sameAs(record->mVerifyData->toString())) {
                        response = CompleteUploadMessage::New();
                    } else {
                        response = CompleteUploadMessage::New(EBADF);
                        //remove corrupted file
                        File file = File::New(record->mPath);
                        file->removeAll();
                    }
                    socket->getOutputStream()->write(mConverter->generatePacket(response));
                }
            }
        } break;

        case st(Net)::Event::Disconnect: {
            auto record = mRecords->remove(socket);
            mMonitor->unbind(socket);
            if(record->mOutputStream != nullptr) {
                record->mOutputStream->close();
            }
            //check whether file finish uploading
            if(record->mFileSize != 0 && record->mPath != nullptr) {
                File file = File::New(record->mPath);
                if(file->exists()) {
                    file->removeAll();
                }
            }
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
    mConverter = DistributeMessageConverter::New();
    mDownloadPool = ExecutorBuilder::New()->setMinThreadNum(2)->newCachedThreadPool();
    mSavedPath = option->getSavedPath();
    mThreads = ConcurrentQueue<ArchiveCenterUploadMonitor>::New();
    //mDownloadRequests = ConcurrentHashMap<DistributeLinker,File>::New();
    mReadLinks = ConcurrentHashMap<DistributeLinker,FileInputStream>::New();
    mWriteLinks = ConcurrentHashMap<DistributeLinker,FileOutputStream>::New();

    mOption = option;
    mCurrentPort = AtomicUint32::New(mOption->getMonitorStartPort());
    mHandler = mOption->getHandler();

    //mFileNoGenerator = AtomicUint64::New(0);
    //mDownloadIdMap = ConcurrentHashMap<Integer,File>::New();

    mFileManager = ArchiveFileManager::New();
}

_ArchiveCenter::~_ArchiveCenter() {

}

int _ArchiveCenter::onMessage(DistributeLinker linker,ByteArray data) {
    auto msg = mConverter->generateMessage<ArchiveMessage>(data);
    switch(msg->getEvent()) {
        case st(ArchiveMessage)::ApplyUploadConnect: {
            FetchRet(ret,path) = transformFilePath(linker,msg);
            if(ret == st(ArchiveHandleResult)::Reject) {
                auto response = ConfirmReadMessage::New(EPERM);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                break;
            }

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
            ConfirmUploadConnectMessage response = ConfirmUploadConnectMessage::New(selected->getPort());
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(ArchiveMessage)::ApplyOpen: {
            FetchRet(ret,path) = transformFilePath(linker,msg);
            if(ret == st(ArchiveHandleResult)::Reject) {
                auto response = ConfirmOpenMessage::New(0,EPERM);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                break;
            }
            
            File file = File::New(path);
            ConfirmOpenMessage resp = nullptr;
            if(!file->exists()) {
                resp = ConfirmOpenMessage::New(0,ENOENT);
            } else {
                path = file->getAbsolutePath();
                //update status
                uint64_t flags = msg->getFlags();
                if(flags == O_RDONLY) {
                    auto permitFlag = mFileManager->updateFileStatus(path,linker,st(ArchiveFileManager)::Action::Read);
                    if(permitFlag == 0) {
                        auto input = FileInputStream::New(path);
                        input->open(); //TODO? open result check
                        auto fileno = mFileManager->addReadLink(linker,input);
                        resp = ConfirmOpenMessage::New(fileno,0);
                        mFileManager->addOpenFileLink(fileno,file);
                    } else {
                        resp = ConfirmOpenMessage::New(0,EACCES);
                    }
                } else {
                    auto permitFlag = mFileManager->updateFileStatus(path,linker,st(ArchiveFileManager)::Action::Write);
                    if(permitFlag == 0) {
                        auto output = FileOutputStream::New(path);
                        if(flags & O_APPEND) {
                            output->open(st(IO)::FileControlFlags::Append);
                        } else {
                            output->open(st(IO)::FileControlFlags::Trunc);
                        }
                        auto fileno = mFileManager->addWriteLink(linker,output);
                        resp = ConfirmOpenMessage::New(fileno,0);
                        mFileManager->addOpenFileLink(fileno,file);
                    } else {
                        resp = ConfirmOpenMessage::New(0,EACCES);
                    }
                }
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(resp));
        } break;

        case st(ArchiveMessage)::ApplySeekTo: {
            ConfirmSeekToMessage response = nullptr;
            if(msg->getSeekType() == st(ApplySeekToMessage)::Read) {
                auto input = mFileManager->getInputStream(linker,msg->getFileNo());
                if(input != nullptr) {
                    int ret = input->seekTo(msg->getStartPos());
                    response = ConfirmSeekToMessage::New(ret > 0?0:EIO);
                }
            } else {
                //TODO outputstream does not support seek to.....
                // auto output = mFileManager->getOutputStream(linker,msg->getFileNo());
                // if(output != nullptr) {
                //     int ret = output->seekTo(msg->getStartPos());
                //     response = ConfirmSeekToMessage::New(ret > 0?0:EIO);
                // }
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(ArchiveMessage)::ProcessRead: {
            auto input = mFileManager->getInputStream(linker,msg->getFileNo());
            uint64_t length = msg->getReadLength();
            if(input == nullptr) {
                auto resp = ConfirmReadMessage::New(ENOENT);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(resp));
            } else {
                mDownloadPool->submit([&](DistributeLinker mylinker,FileInputStream input,uint64_t length){
                    ByteArray data = ByteArray::New(length);
                    int ret = input->read(data);
                    auto resp = ConfirmReadMessage::New(data);
                    mylinker->getSocket()->getOutputStream()->write(mConverter->generatePacket(resp));
                },linker,input,length);
            }
        } break;

        case st(ArchiveMessage)::ApplyDel: {
            FetchRet(ret,path) = transformFilePath(linker,msg);
            ConfirmDelMessage response = nullptr;
            do {
                if(ret == st(ArchiveHandleResult)::Reject) {
                    response = ConfirmDelMessage::New(EPERM);
                    break;
                }

                File file = File::New(path);
                if(!file->exists()) {
                    response = ConfirmDelMessage::New(ENOENT);
                } else {
                    path = file->getAbsolutePath();
                    auto permitFlag = mFileManager->updateFileStatus(path,linker,st(ArchiveFileManager)::Action::Delete);
                    if(permitFlag == -1) {
                        response = ConfirmDelMessage::New(EACCES);
                    } else {
                        file->removeAll();
                        response = ConfirmDelMessage::New();
                        mFileManager->removeFileStatus(path,linker,st(ArchiveFileManager)::Action::Delete);
                    }
                }
            } while(0);

            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(ArchiveMessage)::ApplyRename: {
            FetchRet(ret,path) = transformFilePath(linker,msg);
            ConfirmRenameMessage response = nullptr;
            do {
                if(ret == st(ArchiveHandleResult)::Reject) {
                    response = ConfirmRenameMessage::New(EPERM);
                    break;
                }

                File file = File::New(path);
                
                if(!file->exists()) {
                    response = ConfirmRenameMessage::New(ENOENT);
                } else {
                    path = file->getAbsolutePath();
                    auto permitFlag = mFileManager->updateFileStatus(path,linker,st(ArchiveFileManager)::Action::Rename);
                    if(permitFlag == -1) {
                        response = ConfirmRenameMessage::New(EACCES);
                    } else {
                        String newname = msg->getData()->toString();
                        response = ConfirmRenameMessage::New(file->rename(newname));
                        mFileManager->removeFileStatus(path,linker,st(ArchiveFileManager)::Action::Rename);
                    }
                }
            } while(0);
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(ArchiveMessage)::ApplyWrite: {
            //auto outputStream = mWriteLinks->get(linker);
            auto outputStream = mFileManager->getOutputStream(linker,msg->getFileNo());
            ConfirmWriteMessage resp = ConfirmWriteMessage::New();
            if(outputStream != nullptr) {
                outputStream->write(msg->getData());
            } else {
                resp = ConfirmWriteMessage::New(EPIPE);
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(resp));
        } break;

        case st(ArchiveMessage)::ApplyDownload: {
            FetchRet(ret,path) = transformFilePath(linker,msg);//getDownloadFilePath(msg->filename);
            if(ret == st(ArchiveHandleResult)::Reject) {
                auto response = ConfirmDownloadMessage::New(EPERM);
                linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                break;
            }

            File file = File::New(path);
            ConfirmDownloadMessage msg = nullptr;
            if(file->exists()) {
                if(mFileManager->updateFileStatus(file->getAbsolutePath(),
                                                  linker,
                                                  st(ArchiveFileManager)::Action::Download) != -1) {
                    path = file->getAbsolutePath();
                    auto fileno = mFileManager->addDownloadLink(file);
                    msg = ConfirmDownloadMessage::New(file,fileno);
                } else {
                    msg = ConfirmDownloadMessage::New(EPERM);
                }
            } else {
                msg = ConfirmDownloadMessage::New(ENOENT);
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(msg));
        } break;

        case st(ArchiveMessage)::ProcessDownload: {
            auto fileno = msg->getFileNo();
            mDownloadPool->submit([&](DistributeLinker mylinker,uint64_t fileno){
                ByteArray buff = ByteArray::New(32*1024);
                File file = mFileManager->getDownloadFile(fileno);
                if(file == nullptr) {
                    return;
                }

                FileInputStream stream = FileInputStream::New(file);
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
                mFileManager->removeDownloadFile(fileno);
                mFileManager->removeFileStatus(file->getAbsolutePath(),mylinker,st(ArchiveFileManager)::Action::Download);
            },linker,fileno);
        } break;

        case st(ArchiveMessage)::QueryInfo: {
            FetchRet(ret,path) = transformFilePath(linker,msg);
            ConfirmQueryInfoMessage msg = nullptr;
            if(ret != st(ArchiveHandleResult)::Type::Reject) {
                File file = File::New(path);
                msg = ConfirmQueryInfoMessage::New(file);
            } else {
                msg = ConfirmQueryInfoMessage::New(EPERM);
            }
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(msg));
        } break;

        case st(ArchiveMessage)::ApplyCloseStream: {
            auto outputStream = mFileManager->getOutputStream(linker,msg->getFileNo());
            auto response = ConfirmCloseStreamMessage::New();
            auto file = mFileManager->getOpenFile(msg->getFileNo());
            auto fileno = msg->getFileNo();
            if(outputStream != nullptr) {
                mFileManager->removeWriteLink(linker,fileno);
                mFileManager->removeFileStatus(file->getAbsolutePath(),linker,st(ArchiveFileManager)::Action::Write);
                mFileManager->removeOpenFile(fileno);
                outputStream->close();
            } else {
                auto inputstream = mFileManager->getInputStream(linker,fileno);
                if(inputstream != nullptr) {
                    mFileManager->removeFileStatus(file->getAbsolutePath(),linker,st(ArchiveFileManager)::Action::Read);
                    mFileManager->removeOpenFile(fileno);
                    inputstream->close();
                } else {
                    response = ConfirmCloseStreamMessage::New(ENOENT);
                }   
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
                addr = Inet4Address::New(addrStr,port);
            } break;

            case st(Net)::Family::Ipv6: {
                addr = Inet6Address::New(addrStr,port);
            } break;

            case st(Net)::Family::Local: {
                //TODO
            } break;
        }
        
        ServerSocket sock = SocketBuilder::New()->setAddress(addr)->newServerSocket();
        if(sock->bind() < 0) {
            continue;
        }

        return ArchiveCenterUploadMonitor::New(sock,mOption->getSavedPath(),this);
    }

    return nullptr;
}

DefRet(st(ArchiveHandleResult)::Type,String) _ArchiveCenter::transformFilePath(DistributeLinker link,ArchiveMessage msg) {
    String path = nullptr;
    st(ArchiveHandleResult)::Type type = st(ArchiveHandleResult)::Type::Deal;
    if(mHandler != nullptr) {
        ArchiveHandleResult ret = mHandler->onRequest(link,msg);
        if(ret != nullptr) {
            path = ret->path;
            type = ret->result;
        }
    }

    if(path == nullptr && type == st(ArchiveHandleResult)::Type::Deal) {
        path = mSavedPath->append("/",msg->getFileName());
    }
    return MakeRet(type,path);
}

}
