#include "DocumentCenter.hpp"
#include "FileInputStream.hpp"
#include "ForEveryOne.hpp"
#include "SocketBuilder.hpp"
#include "Inet4Address.hpp"
#include "Inet6Address.hpp"
#include "InetLocalAddress.hpp"
#include "DocumentMessage.hpp"

namespace gagira {

//----DocumentCenterUploadMonitor---
const int _DocumentCenterUploadMonitor::kBusyLevel = 32;

_DocumentCenterUploadMonitor::_DocumentCenterUploadMonitor(ServerSocket socket,String savedPath,DocumentHandler h) {
    mServer = socket;
    mMonitor = createSocketMonitor();
    mMonitor->bind(mServer,AutoClone(this));
    mLinkers = createConcurrentHashMap<Socket,DocumentLinker>();
    mConverter = createDistributeMessageConverter();
    mSavedPath = savedPath;
    mHandler = h;
}

void _DocumentCenterUploadMonitor::onSocketMessage(int event,Socket socket,ByteArray array) {
    switch(event) {
        case st(NetEvent)::Message: {
            DocumentLinker linker = mLinkers->get(socket);
            if(linker == nullptr) {
                linker = createDocumentLinker(socket);
                mLinkers->put(socket,linker);
                linker->setStatus(st(DocumentLinker)::WaitClientApplyInfo);
            }
            if(linker->getStatus() == st(DocumentLinker)::WaitClientApplyInfo) {
                auto response = linker->doParse(array);
                if(response != nullptr && response->size() != 0) {
                    ByteArray applyInfoData = response->get(0);
                    auto msg = mConverter->generateMessage<ApplyUploadInfoMessage>(applyInfoData);
                    String path = nullptr;
                    if(mHandler != nullptr) {
                        path = mHandler->onUploadFile(msg->filename);
                    }

                    ConfirmApplyUploadInfoMessage response = nullptr;
                    if(path == nullptr) {
                        path = mSavedPath->append("/",msg->filename);
                    } else if(path->equals(st(DocumentMessage)::kReject)) {
                        response = createConfirmApplyUploadInfoMessage();
                        response->data = st(DocumentMessage)::kReject->toByteArray();
                        linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                        break;
                    }

                    File file = createFile(path);
                    
                    if(!file->exists()) {
                        file->createNewFile();
                        linker->setFileSize(msg->length)
                            ->setPath(file->getAbsolutePath());
                        response = createConfirmApplyUploadInfoMessage();
                        linker->setStatus(st(DocumentLinker)::WaitClientMessage);
                    } else {
                        response = createConfirmApplyUploadInfoMessage();
                        response->length = file->length();
                        mMonitor->unbind(linker->getSocket(),false);
                        mLinkers->remove(socket);
                    }
                    linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
                } 
            } else if(linker->getStatus() == st(DocumentLinker)::WaitClientMessage) {
                linker->getOutputStream()->write(array);
                if(linker->reduceFileSize(array->size()) == 0) {
                    mMonitor->unbind(linker->getSocket(),false);
                    mLinkers->remove(socket);
                }
            }
        } break;

        case st(NetEvent)::Disconnect: {
            mMonitor->unbind(socket,false);
            mLinkers->remove(socket);
        } break;
    }
}

void _DocumentCenterUploadMonitor::bind(DocumentLinker linker) {
    mLinkers->put(linker->getSocket(),linker);
    mMonitor->bind(linker->getSocket(),AutoClone(this));
}

uint32_t _DocumentCenterUploadMonitor::getPort() {
    return mServer->getInetAddress()->getPort();
}

bool _DocumentCenterUploadMonitor::isBusy() {
    return mLinkers->size() > kBusyLevel;
}

//--- DocumentCenter ---
_DocumentCenter::_DocumentCenter(String url,DocumentOption option):_DistributeCenter(url,Cast<DistributeOption>(option)) {
    mConverter = createDistributeMessageConverter();
    mDownloadPool = createExecutorBuilder()->setMinThreadNum(2)->newCachedThreadPool();
    mSavedPath = option->getSavedPath();
    mThreads = createConcurrentQueue<DocumentCenterUploadMonitor>();
    mDownloadRequests = createConcurrentHashMap<DistributeLinker,File>();
    mOption = option;
    mCurrentPort = createAtomicUint32(mOption->getMonitorStartPort());
    mHandler = mOption->getHandler();
}

_DocumentCenter::~_DocumentCenter() {

}

int _DocumentCenter::onMessage(DistributeLinker linker,ByteArray data) {
    auto msg = mConverter->generateMessage<DocumentMessage>(data);
    switch(msg->event) {
        case st(DocumentMessage)::ApplyUpload: {
            DocumentCenterUploadMonitor selected = nullptr;

            ForEveryOne(m,mThreads) {
                if(!m->isBusy()) {
                    selected = m;
                }
            }

            if(selected == nullptr) {
                selected = createUploadMonitor();
                mThreads->add(selected);
            }
            ConfirmUploadMessage response = createConfirmUploadMessage(selected->getPort());
            linker->getSocket()->getOutputStream()->write(mConverter->generatePacket(response));
        } break;

        case st(DocumentMessage)::ApplyDownload: {
            String path = nullptr;
            if(mHandler != nullptr) {
                path = mHandler->onDownloadFile(msg->filename);
            }

            if(path == nullptr) {
                path = mSavedPath->append("/",msg->filename);
            } else if(path->equals(st(DocumentMessage)::kReject)) {
                auto response = createConfirmDownloadMessage();
                response->data = st(DocumentMessage)::kReject->toByteArray();
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

        case st(DocumentMessage)::ProcessDownload: {
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
    }

    return 0;
}

int _DocumentCenter::onNewClient(DistributeLinker linker) {
    return 0;
}

int _DocumentCenter::onDisconnectClient(DistributeLinker linker) {
    //TODO
    return 0;
}

DocumentCenterUploadMonitor _DocumentCenter::createUploadMonitor() {
    while(1) {
        uint32_t port = mCurrentPort->getAndIncrement();
        if(port > mOption->getMonitorEndPort()) {
            mCurrentPort->set(mOption->getMonitorStartPort());
            return nullptr;
        }
        String addrStr = mAddress->getAddress();
        InetAddress addr = nullptr;
        switch(mAddress->getFamily()) {
            case st(InetAddress)::IPV4: {
                addr = createInet4Address(addrStr,port);
            } break;

            case st(InetAddress)::IPV6: {
                addr = createInet6Address(addrStr,port);
            } break;

            case st(InetAddress)::LOCAL: {
                //TODO
            } break;
        }
        
        ServerSocket sock = createSocketBuilder()->setAddress(addr)->newServerSocket();
        if(sock->bind() < 0) {
            continue;
        }

        return createDocumentCenterUploadMonitor(sock,mOption->getSavedPath(),mHandler);
    }

    return nullptr;
}

}
