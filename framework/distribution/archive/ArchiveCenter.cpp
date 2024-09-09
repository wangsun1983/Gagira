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
#include "Log.hpp"

namespace gagira {

//----ArchiveCenterUploadMonitor---
const int _ArchiveCenterUploadMonitor::kBusyLevel = 32;

_ArchiveCenterUploadRecord::_ArchiveCenterUploadRecord() {
    mParser = DistributeMessageParser::New(1024*32);
    mFileSize = 0;
}

void _ArchiveCenterUploadMonitor::notifyRelease(CountDownLatch l) {
    AutoLock ll(mStatusMutex);
    if(mProcessStatus == ProcessIdle) {
        l->countDown();
        mProcessStatus = ProcessClose;
    } else {
        mCloseLatch = l;
    }
}

void _ArchiveCenterUploadMonitor::release() {
    mMonitor->close();
    //remove files if they are not uploaded completely
    ForEveryOne(pair,mRecords) {
        auto record = pair->getValue();
        if(record->mPath != nullptr) {
            File file = File::New(record->mPath);
            file->removeAll();
        }
    }
}

_ArchiveCenterUploadMonitor::_ArchiveCenterUploadMonitor(ServerSocket socket,String savedPath,ArchiveFileManager mgr,_ArchiveCenter *c) {
    mServer = socket;
    mMonitor = SocketMonitor::New();
    mMonitor->bind(mServer,AutoClone(this));
    mRecords = ConcurrentHashMap<Socket,ArchiveCenterUploadRecord>::New();

    mConverter = DistributeMessageConverter::New();
    mSavedPath = savedPath;
    mCenter = c;
    mFileManager = mgr;
    mStatusMutex = Mutex::New();
    mProcessStatus = ProcessIdle;
}

void _ArchiveCenterUploadMonitor::responseFail(int reason,ArchiveCenterUploadRecord record) {
    auto response = ConfirmApplyUploadMessage::New(reason);
    auto sock = record->mLinker->getSocket();
    auto out = sock->getOutputStream();
    if(out != nullptr) {
        out->write(mConverter->generatePacket(response));
        out->flush();
    }
    mRecords->remove(sock);
    //mMonitor->unbind(sock);
    if(record->mOutputStream != nullptr) {
        record->mOutputStream->close();
    }

    if(record->mPath != nullptr) {
        File file = File::New(record->mPath);
        if(file->exists()) {
            file->removeAll();
        }
    }

    mFileManager->removeFileStatus(record->mPath,record->mLinker,st(ArchiveFileManager)::Action::Upload);
}

void _ArchiveCenterUploadMonitor::onSocketMessage(st(Net)::Event event,Socket socket,ByteArray array) {
    {
        AutoLock l(mStatusMutex);
        if(mProcessStatus == ProcessClose) {
            return;
        }

        mProcessStatus = ProcessRunning;
    }
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
                    record->mLinker = DistributeLinker::New(socket);

                    ByteArray applyInfoData = response->get(0);
                    auto msg = mConverter->generateMessage<ApplyUploadMessage>(applyInfoData);
                    FetchRet(ret,path) = mCenter->transformFilePath(DistributeLinker::New(socket),msg);
                    ConfirmApplyUploadMessage response = nullptr;
                    if(ret == st(ArchiveHandleResult)::Reject) {
                        responseFail(EPERM,record);
                        break;
                    }

                    //update file status
                    File file = File::New(path);
                    if(!file->exists()) {
                        file->createNewFile();
                        record->mPath = file->getAbsolutePath();
                        auto result = mFileManager->updateFileStatus(path,
                                                                     record->mLinker,
                                                                     st(ArchiveFileManager)::Action::Upload);
                        if(result != 0) {
                            responseFail(-result,record);
                            break;
                        } else {
                            record->mFileSize = msg->getUploadFileLength();
                            record->mOutputStream = FileOutputStream::New(file);
                            if(!record->mOutputStream->open()) {
                                responseFail(EBADF,record);
                                break;
                            }
                            response = ConfirmApplyUploadMessage::New();
                            record->mStatus = WaitClientMessage;
                            record->mVerifyData = msg->getVerifyData();
                            printf("record mVerifyData is %s \n",record->mVerifyData->toString()->toChars());
                        }
                    } else {
                        //response = ConfirmApplyUploadMessage::New(EEXIST);
                        responseFail(EEXIST,record);
                        break;
                    }
                    socket->getOutputStream()->write(mConverter->generatePacket(response));
                }
            } else if(record->mStatus == WaitClientMessage) {
                int ret = record->mOutputStream->write(array);
                record->mFileSize -= array->size();
                if(record->mFileSize == 0) {
                    //check current file md5sum
                    Md md5sum = Md::New();
                    auto mdValue = md5sum->encodeFile(File::New(record->mPath));
                    CompleteUploadMessage response = nullptr;
                    printf("upload mdvalue is %s,verifydata is %s \n",mdValue->toChars(),record->mVerifyData->toString()->toChars());
                    if(mdValue->sameAs(record->mVerifyData->toString())) {
                        printf("md5 check ok \n");
                        response = CompleteUploadMessage::New();
                        mFileManager->removeFileStatus(record->mPath,record->mLinker,st(ArchiveFileManager)::Action::Upload);
                        socket->getOutputStream()->write(mConverter->generatePacket(response));
                    } else {
                        printf("md5 check fail \n");
                        //response = CompleteUploadMessage::New(EBADF);
                        //remove corrupted file
                        File file = File::New(record->mPath);
                        file->removeAll();
                        responseFail(EBADF,record);
                    }
                }
            }
        } break;

        case st(Net)::Event::Disconnect: {
            auto record = mRecords->remove(socket);
            mMonitor->unbind(socket);
            if(record != nullptr) {
                if(record->mOutputStream != nullptr) {
                    record->mOutputStream->close();
                }

                //check whether file finish uploading
                if(record->mFileSize != 0 && record->mPath != nullptr) {
                    File file = File::New(record->mPath);
                    if(file->exists()) {
                        file->removeAll();
                    }
                    //update file status
                }
                mFileManager->onLinkerDisconnected(record->mLinker);
            }
        } break;
    }

    {
        AutoLock l(mStatusMutex);
        if(mCloseLatch != nullptr) {
            mProcessStatus = ProcessClose;
            mCloseLatch->countDown();
        } else {
            mProcessStatus = ProcessIdle;
        }
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
    //mReadLinks = ConcurrentHashMap<DistributeLinker,FileInputStream>::New();
    //mWriteLinks = ConcurrentHashMap<DistributeLinker,FileOutputStream>::New();

    mOption = option;
    mCurrentPort = AtomicUint32::New(mOption->getMonitorStartPort());
    mHandler = mOption->getHandler();
    mFileManager = ArchiveFileManager::New();
}

_ArchiveCenter::~_ArchiveCenter() {

}

int _ArchiveCenter::processApplyUploadConnect(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
    FetchRet(ret,path) = transformFilePath(linker,msg);
    if(ret == st(ArchiveHandleResult)::Reject) {
        auto response = ConfirmReadMessage::New(EPERM);
        return out->write(mConverter->generatePacket(response));
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
    return out->write(mConverter->generatePacket(response));
}

int _ArchiveCenter::processApplyOpen(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
    FetchRet(ret,path) = transformFilePath(linker,msg);
    if(ret == st(ArchiveHandleResult)::Reject) {
        auto response = ConfirmOpenMessage::New(st(ArchiveFileManager)::kInvalidFileNo,EPERM);
        return out->write(mConverter->generatePacket(response));
    }
    
    File file = File::New(path);
    ConfirmOpenMessage resp = nullptr;
    if(!file->exists()) {
        resp = ConfirmOpenMessage::New(st(ArchiveFileManager)::kInvalidFileNo,ENOENT);
    } else {
        path = file->getAbsolutePath();
        //update status
        uint64_t flags = msg->getFlags();
        if(flags == O_RDONLY) {
            auto result = mFileManager->updateFileStatus(path,linker,st(ArchiveFileManager)::Action::Read);
            if(result == 0) {
                auto input = FileInputStream::New(path);
                input->open(); 
                auto fileno = mFileManager->addReadLink(linker,input);
                resp = ConfirmOpenMessage::New(fileno,0);
                mFileManager->addOpenFileLink(linker,fileno,file);
            } else {
                resp = ConfirmOpenMessage::New(st(ArchiveFileManager)::kInvalidFileNo,-result);
            }
        } else {
            auto result = mFileManager->updateFileStatus(path,linker,st(ArchiveFileManager)::Action::Write);
            if(result == 0) {
                auto output = FileOutputStream::New(path);
                if(flags & O_APPEND) {
                    output->open(st(IO)::FileControlFlags::Append);
                } else {
                    output->open(st(IO)::FileControlFlags::Trunc);
                }
                auto fileno = mFileManager->addWriteLink(linker,output);
                resp = ConfirmOpenMessage::New(fileno,0);
                mFileManager->addOpenFileLink(linker,fileno,file);
            } else {
                resp = ConfirmOpenMessage::New(st(ArchiveFileManager)::kInvalidFileNo,-result);
            }
        }
    }
    return out->write(mConverter->generatePacket(resp));
}

int _ArchiveCenter::processApplySeekTo(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
    ConfirmSeekToMessage response = nullptr;
    if(msg->getSeekType() == st(ApplySeekToMessage)::Read) {
        auto input = mFileManager->getInputStream(linker,msg->getFileNo());
        if(input != nullptr) {
            int ret = input->seekTo(msg->getStartPos());
            response = ConfirmSeekToMessage::New(ret > 0?0:EIO);
        } else {
            response = ConfirmSeekToMessage::New(EINVAL);
        }
    } else {
        //TODO outputstream does not support seek to.....
        // auto output = mFileManager->getOutputStream(linker,msg->getFileNo());
        // if(output != nullptr) {
        //     int ret = output->seekTo(msg->getStartPos());
        //     response = ConfirmSeekToMessage::New(ret > 0?0:EIO);
        // }
    }
    return out->write(mConverter->generatePacket(response));
}

int _ArchiveCenter::processApplyRead(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
    auto input = mFileManager->getInputStream(linker,msg->getFileNo());
    uint64_t length = msg->getReadLength();
    if(input == nullptr) {
        auto resp = ConfirmReadMessage::New(ENOENT);
        return out->write(mConverter->generatePacket(resp));
    } else {
        mDownloadPool->submit([&](DistributeLinker mylinker,FileInputStream input,uint64_t length){
            ByteArray data = ByteArray::New(length);
            int ret = input->read(data);
            auto resp = ConfirmReadMessage::New(data);
            mylinker->getSocket()->getOutputStream()->write(mConverter->generatePacket(resp));
        },linker,input,length);
    }
    return 0;
}

int _ArchiveCenter::processApplyDel(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
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
            auto result = mFileManager->updateFileStatus(path,linker,st(ArchiveFileManager)::Action::Delete);
            if(result == 0) {
                file->removeAll();
                response = ConfirmDelMessage::New();
                mFileManager->removeFileStatus(path,linker,st(ArchiveFileManager)::Action::Delete);
            } else {
                response = ConfirmDelMessage::New(-result);
            }
        }
    } while(0);

    return out->write(mConverter->generatePacket(response));
}

int _ArchiveCenter::processApplyRename(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
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
            auto result = mFileManager->updateFileStatus(path,linker,st(ArchiveFileManager)::Action::Rename);
            if(result == 0) {
                String newname = msg->getData()->toString();
                response = ConfirmRenameMessage::New(file->rename(newname));
                mFileManager->removeFileStatus(path,linker,st(ArchiveFileManager)::Action::Rename);
            } else {
                response = ConfirmRenameMessage::New(-result);
            }
        }
    } while(0);
    return out->write(mConverter->generatePacket(response));
}

int _ArchiveCenter::processApplyWrite(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
    auto outputStream = mFileManager->getOutputStream(linker,msg->getFileNo());
    ConfirmWriteMessage resp = nullptr;
    if(outputStream != nullptr) {
        outputStream->write(msg->getData());
        resp = ConfirmWriteMessage::New();
    } else {
        resp = ConfirmWriteMessage::New(EINVAL);
    }
    return out->write(mConverter->generatePacket(resp));
}

int _ArchiveCenter::processApplyDownload(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
    FetchRet(ret,path) = transformFilePath(linker,msg);//getDownloadFilePath(msg->filename);
    if(ret == st(ArchiveHandleResult)::Reject) {
        auto response = ConfirmDownloadMessage::New(EPERM);
        return out->write(mConverter->generatePacket(response));
    }

    File file = File::New(path);
    ConfirmDownloadMessage response = nullptr;
    if(file->exists()) {
        auto result = mFileManager->updateFileStatus(file->getAbsolutePath(),
                                            linker,
                                            st(ArchiveFileManager)::Action::Download);
        if(result == 0) {
            path = file->getAbsolutePath();
            auto fileno = mFileManager->addDownloadLink(linker,file);
            response = ConfirmDownloadMessage::New(file,fileno);
        } else {
            response = ConfirmDownloadMessage::New(-result);
        }
    } else {
        response = ConfirmDownloadMessage::New(ENOENT);
    }
    return out->write(mConverter->generatePacket(response));
}

int _ArchiveCenter::processDoDownload(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
    auto fileno = msg->getFileNo();
    mDownloadPool->submit([&](DistributeLinker mylinker,uint64_t fileno){
        ByteArray buff = ByteArray::New(32*1024);
        File file = mFileManager->getDownloadFile(mylinker,fileno);
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
        mFileManager->removeDownloadFile(mylinker,fileno);
        mFileManager->removeFileStatus(file->getAbsolutePath(),mylinker,st(ArchiveFileManager)::Action::Download);
    },linker,fileno);

    return 0;
}

int _ArchiveCenter::processApplyQueryInfo(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
    FetchRet(ret,path) = transformFilePath(linker,msg);
    ConfirmQueryInfoMessage response = nullptr;
    if(ret != st(ArchiveHandleResult)::Type::Reject) {
        File file = File::New(path);
        response = ConfirmQueryInfoMessage::New(file);
    } else {
        response = ConfirmQueryInfoMessage::New(EPERM);
    }
    return out->write(mConverter->generatePacket(response));
}

int _ArchiveCenter::processApplyCloseStream(DistributeLinker linker,ArchiveMessage msg,OutputStream out) {
    auto outputStream = mFileManager->getOutputStream(linker,msg->getFileNo());
    auto response = ConfirmCloseStreamMessage::New();
    auto file = mFileManager->getOpenFile(linker,msg->getFileNo());
    auto fileno = msg->getFileNo();
    if(outputStream != nullptr) {
        mFileManager->removeWriteLink(linker,fileno);
        mFileManager->removeFileStatus(file->getAbsolutePath(),linker,st(ArchiveFileManager)::Action::Write);
        mFileManager->removeOpenFile(linker,fileno);
        outputStream->close();
    } else {
        auto inputstream = mFileManager->getInputStream(linker,fileno);
        if(inputstream != nullptr) {
            mFileManager->removeFileStatus(file->getAbsolutePath(),linker,st(ArchiveFileManager)::Action::Read);
            mFileManager->removeOpenFile(linker,fileno);
            inputstream->close();
        } else {
            response = ConfirmCloseStreamMessage::New(ENOENT);
        }   
    }
    return out->write(mConverter->generatePacket(msg));
}

int _ArchiveCenter::onMessage(DistributeLinker linker,ByteArray data) {
    auto msg = mConverter->generateMessage<ArchiveMessage>(data);
    auto out = linker->getSocket()->getOutputStream();
    //this linker maybe closed,return directly
    Inspect(out == nullptr,0)

    switch(msg->getEvent()) {
        case st(ArchiveMessage)::ApplyUploadConnect: {
            processApplyUploadConnect(linker,msg,out);
        } break;

        case st(ArchiveMessage)::ApplyOpen: {
            processApplyOpen(linker,msg,out);
        } break;

        case st(ArchiveMessage)::ApplySeekTo: {
            processApplySeekTo(linker,msg,out);
        } break;

        case st(ArchiveMessage)::ProcessRead: {
            processApplyRead(linker,msg,out);
        } break;

        case st(ArchiveMessage)::ApplyDel: {
            processApplyDel(linker,msg,out);
        } break;

        case st(ArchiveMessage)::ApplyRename: {
            processApplyRename(linker,msg,out);
        } break;

        case st(ArchiveMessage)::ApplyWrite: {
            processApplyWrite(linker,msg,out);
        } break;

        case st(ArchiveMessage)::ApplyDownload: {
            processApplyDownload(linker,msg,out);
        } break;

        case st(ArchiveMessage)::ProcessDownload: {
            processDoDownload(linker,msg,out);
        } break;

        case st(ArchiveMessage)::QueryInfo: {
            processApplyQueryInfo(linker,msg,out);
        } break;

        case st(ArchiveMessage)::ApplyCloseStream: {
            processApplyCloseStream(linker,msg,out);
        } break;
    }

    return 0;
}

int _ArchiveCenter::onNewClient(DistributeLinker linker) {
    //do nothing
    return 0;
}

int _ArchiveCenter::onDisconnectClient(DistributeLinker linker) {
    // auto input = mReadLinks->remove(linker);
    // if(input != nullptr) {
    //     input->close();
    // }

    // auto output = mWriteLinks->remove(linker);
    // if(output != nullptr) {
    //     output->close();
    // }

    mFileManager->onLinkerDisconnected(linker);
    return 0;
}

ArchiveCenterUploadMonitor _ArchiveCenter::createUploadMonitor() {
    do {
        uint32_t port = mCurrentPort->getAndIncrement();
        if(port > mOption->getMonitorEndPort()) {
            mCurrentPort->set(mOption->getMonitorStartPort());
            LOG(ERROR)<<"ArchiveCenterUploadMonitor port is drained!!";
            continue;
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
                //TODO if local,use 127.0.0.1,haha
            } break;
        }
        
        ServerSocket sock = SocketBuilder::New()->setAddress(addr)->newServerSocket();
        if(sock->bind() < 0) {
            continue;
        }

        return ArchiveCenterUploadMonitor::New(sock,mOption->getSavedPath(),mFileManager,this);
    } while(0);

    return nullptr;
}

DefRet(st(ArchiveHandleResult)::Type,String) _ArchiveCenter::transformFilePath(DistributeLinker link,ArchiveMessage msg) {
    if(mHandler != nullptr) {
        ArchiveHandleResult ret = mHandler->onRequest(link,msg);
        if(ret != nullptr) {
            return MakeRet(ret->result,ret->path);
        }
    }

    return MakeRet(st(ArchiveHandleResult)::Type::Deal,mSavedPath->append("/",msg->getFileName()));
}

size_t _ArchiveCenter::getReadLinkNums() {
    return mFileManager->getReadLinkNums();
}

size_t _ArchiveCenter::getWriteLinkNums() {
    return mFileManager->getWriteLinkNums();
}

size_t _ArchiveCenter::getDownloadLinkNums() {
    return mFileManager->getDownloadLinkNums();
}

size_t _ArchiveCenter::getOpenLinkNums() {
    return mFileManager->getOpenLinkNums();
}

int _ArchiveCenter::close() {
    Inspect(st(DistributeCenter)::isClosed(),0)

    st(DistributeCenter)::close();
    //we should close all upload thread
    auto size = mThreads->size();
    if (size > 0) {
        auto latch = CountDownLatch::New(size);
        ForEveryOne(monitor,mThreads) {
            monitor->notifyRelease(latch);
        }

        latch->await();

        ForEveryOne(releaseMonitor,mThreads) {
            releaseMonitor->release();
        }
    }

    return 0;
}

}
