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
#include "AtomicNumber.hpp"
#include "DistributeMessageConverter.hpp"
#include "FileInputStream.hpp"
#include "FileOutputStream.hpp"
#include "DistributeHandler.hpp"
#include "ArchiveFileManager.hpp"
#include "CountDownLatch.hpp"

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
    ByteArray mVerifyData;
    DistributeMessageParser mParser;
    DistributeLinker mLinker;
};

DECLARE_CLASS(ArchiveCenterUploadMonitor) IMPLEMENTS (SocketListener) {
public:
    enum Status {
        Idle = 0,
        WaitClientApplyInfo,
        WaitClientMessage,
    };

    _ArchiveCenterUploadMonitor(ServerSocket socket,String savedPath,ArchiveFileManager mgr,_ArchiveCenter *);
    void responseFail(int reason,ArchiveCenterUploadRecord record);
    bool isBusy();
    void notifyRelease(CountDownLatch);
    void release();
    uint32_t getPort();
private:
    enum ProcessStatus {
        ProcessIdle = 0,
        ProcessRunning,
        ProcessClose
    };

    static const int kBusyLevel;
    void onSocketMessage(st(Net)::Event,Socket,ByteArray);

    ServerSocket mServer;
    //ConcurrentHashMap<Socket,ArchiveLinker> mLinkers;
    ConcurrentHashMap<Socket,ArchiveCenterUploadRecord> mRecords;
    SocketMonitor mMonitor;
    DistributeMessageConverter mConverter;
    String mSavedPath;
    ArchiveFileManager mFileManager;
    _ArchiveCenter *mCenter;
    CountDownLatch mCloseLatch;
    Mutex mStatusMutex;
    ProcessStatus mProcessStatus;
};

DECLARE_CLASS(ArchiveCenter) IMPLEMENTS(DistributeCenter) {
public:
    friend class _ArchiveCenterUploadMonitor;

    _ArchiveCenter(String url,ArchiveOption);
    ~_ArchiveCenter();
    int onMessage(DistributeLinker,ByteArray);
    int onNewClient(DistributeLinker);
    int onDisconnectClient(DistributeLinker);
    int close();

    //interface for unit test
    size_t getReadLinkNums();
    size_t getWriteLinkNums();
    size_t getDownloadLinkNums();
    size_t getOpenLinkNums();

private:
    DefRet(st(ArchiveHandleResult)::Type,String) transformFilePath(DistributeLinker,ArchiveMessage msg);

    //process message function
    int processApplyUploadConnect(DistributeLinker linker,ArchiveMessage msg,OutputStream out);
    int processApplyOpen(DistributeLinker linker,ArchiveMessage msg,OutputStream out);
    int processApplySeekTo(DistributeLinker linker,ArchiveMessage msg,OutputStream out);
    int processApplyRead(DistributeLinker linker,ArchiveMessage msg,OutputStream out);
    int processApplyDel(DistributeLinker linker,ArchiveMessage msg,OutputStream out);
    int processApplyRename(DistributeLinker linker,ArchiveMessage msg,OutputStream out);
    int processApplyWrite(DistributeLinker linker,ArchiveMessage msg,OutputStream out);
    int processApplyDownload(DistributeLinker linker,ArchiveMessage msg,OutputStream out);
    int processDoDownload(DistributeLinker linker,ArchiveMessage msg,OutputStream out);
    int processApplyQueryInfo(DistributeLinker linker,ArchiveMessage msg,OutputStream out);
    int processApplyCloseStream(DistributeLinker linker,ArchiveMessage msg,OutputStream out);

    ArchiveCenterUploadMonitor createUploadMonitor();
    DistributeMessageConverter mConverter;
    ThreadCachedPoolExecutor mDownloadPool;
    ArchiveOption mOption;
    AtomicUint32 mCurrentPort;
    ConcurrentQueue<ArchiveCenterUploadMonitor> mThreads;
    //ConcurrentHashMap<DistributeLinker,File> mDownloadRequests;
    //ConcurrentHashMap<DistributeLinker,FileInputStream> mReadLinks;
    //ConcurrentHashMap<DistributeLinker,FileOutputStream> mWriteLinks;
    String mSavedPath;
    DistributeHandler mHandler;

    //int
    //AtomicUint64 mFileNoGenerator;
    ConcurrentHashMap<Integer,File> mFileNoMaps;

    //status manager
    ArchiveFileManager mFileManager;
};


}

#endif
