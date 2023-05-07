#ifndef __GAGRIA_ARCHIVE_CENTOR_HPP__
#define __GAGRIA_ARCHIVE_CENTOR_HPP__

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
#include "ArchiveMessage.hpp"
#include "ExecutorBuilder.hpp"
#include "ServerSocket.hpp"
#include "ConcurrentQueue.hpp"
#include "ArchiveOption.hpp"
#include "InetAddress.hpp"
#include "AtomicUint32.hpp"
#include "DistributeMessageConverter.hpp"
#include "FileInputStream.hpp"
#include "FileOutputStream.hpp"

using namespace obotcha;

namespace gagira {

class _ArchiveCenter;

DECLARE_CLASS(ArchiveCenterUploadRecord) {
public:
    _ArchiveCenterUploadRecord();
    OutputStream mOutputStream;
    uint64_t mFileSize;
    String mPath;
    int mStatus;
    DistributeMessageParser mParser;
};

DECLARE_CLASS(ArchiveCenterUploadMonitor) IMPLEMENTS (SocketListener) {
public:
    enum Status {
        Idle = 0,
        WaitClientApplyInfo,
        WaitClientMessage,
    };

    _ArchiveCenterUploadMonitor(ServerSocket socket,String savedPath,_ArchiveCenter *);
    bool isBusy();
    uint32_t getPort();
private:
    static const int kBusyLevel;
    void onSocketMessage(int,Socket,ByteArray);

    ServerSocket mServer;
    //ConcurrentHashMap<Socket,ArchiveLinker> mLinkers;
    ConcurrentHashMap<Socket,ArchiveCenterUploadRecord> mRecords;
    SocketMonitor mMonitor;
    DistributeMessageConverter mConverter;
    String mSavedPath;
    _ArchiveCenter *mCenter;
};

DECLARE_CLASS(ArchiveCenter) IMPLEMENTS(DistributeCenter) {
public:
    friend class _ArchiveCenterUploadMonitor;

    _ArchiveCenter(String url,ArchiveOption);
    ~_ArchiveCenter();
    int onMessage(DistributeLinker,ByteArray);
    int onNewClient(DistributeLinker);
    int onDisconnectClient(DistributeLinker);

private:
    String transformFilePath(DistributeLinker,ArchiveMessage msg);

    ArchiveCenterUploadMonitor createUploadMonitor();
    DistributeMessageConverter mConverter;
    ThreadCachedPoolExecutor mDownloadPool;
    ArchiveOption mOption;
    AtomicUint32 mCurrentPort;
    ConcurrentQueue<ArchiveCenterUploadMonitor> mThreads;
    ConcurrentHashMap<DistributeLinker,File> mDownloadRequests;
    ConcurrentHashMap<DistributeLinker,FileInputStream> mReadLinks;
    ConcurrentHashMap<DistributeLinker,FileOutputStream> mWriteLinks;
    String mSavedPath;
    DistributeHandler mHandler;
};

}

#endif
