#ifndef __GAGRIA_DOCUMENT_CENTOR_HPP__
#define __GAGRIA_DOCUMENT_CENTOR_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "SocketMonitor.hpp"
#include "InetAddress.hpp"
#include "DistributeCenter.hpp"
#include "DistributeOption.hpp"
#include "DistributeMessageConverter.hpp"
#include "Inet4Address.hpp"
#include "Inet6Address.hpp"
#include "Mutex.hpp"
#include "ArrayList.hpp"
#include "DocumentLinker.hpp"
#include "DocumentMessage.hpp"
#include "ExecutorBuilder.hpp"
#include "ServerSocket.hpp"
#include "ConcurrentQueue.hpp"
#include "DocumentOption.hpp"
#include "InetAddress.hpp"
#include "AtomicUint32.hpp"
#include "DistributeMessageConverter.hpp"

using namespace obotcha;

namespace gagira {

class _DocumentCenter;

DECLARE_CLASS(DocumentCenterUploadMonitor) IMPLEMENTS (SocketListener) {
public:
    _DocumentCenterUploadMonitor(ServerSocket socket,String savedPath,DocumentHandler h);
    void bind(DocumentLinker);
    bool isBusy();
    uint32_t getPort();
private:
    static const int kBusyLevel;
    void onSocketMessage(int,Socket,ByteArray);

    ServerSocket mServer;
    ConcurrentHashMap<Socket,DocumentLinker> mLinkers;
    SocketMonitor mMonitor;
    DistributeMessageConverter mConverter;
    String mSavedPath;
    DocumentHandler mHandler;
};

DECLARE_CLASS(DocumentCenter) IMPLEMENTS(DistributeCenter) {
public:
    _DocumentCenter(String url,DocumentOption);
    ~_DocumentCenter();
    int onMessage(DistributeLinker,ByteArray);
    int onNewClient(DistributeLinker);
    int onDisconnectClient(DistributeLinker);

private:
    DocumentCenterUploadMonitor createUploadMonitor();
    DistributeMessageConverter mConverter;
    ThreadCachedPoolExecutor mDownloadPool;
    DocumentOption mOption;
    AtomicUint32 mCurrentPort;
    ConcurrentQueue<DocumentCenterUploadMonitor> mThreads;
    ConcurrentHashMap<DistributeLinker,File> mDownloadRequests;
    String mSavedPath;
    DocumentHandler mHandler;
};

}

#endif
