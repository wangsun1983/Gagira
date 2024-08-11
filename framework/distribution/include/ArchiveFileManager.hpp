#ifndef __GAGRIA_ARCHIVE_FILE_STATUS_MANAGER_HPP__
#define __GAGRIA_ARCHIVE_FILE_STATUS_MANAGER_HPP__

#include "Object.hpp"
#include "DistributeLinker.hpp"
#include "Mutex.hpp"
#include "HashMap.hpp"
#include "ArchiveFile.hpp"
#include "ArrayList.hpp"
#include "FileInputStream.hpp"
#include "FileOutputStream.hpp"
#include "AtomicNumber.hpp"
#include "Uint64.hpp"

using namespace obotcha;

namespace gagira {


DECLARE_CLASS(ArchiveFileManager) {
public:
    static const uint64_t kInvalidFileNo; //file no start from 1    
    enum Action {
        None = 0,
        Upload,
        Download,
        Delete,
        Rename,
        Move,
        Copy,
        Write,
        Read,
        Max
    };

    _ArchiveFileManager();
    int updateFileStatus(String path,DistributeLinker linker,Action action);
    void removeFileStatus(String path,DistributeLinker linker,Action action);
    bool isFileOwner(String path,DistributeLinker linker);

    uint64_t addReadLink(DistributeLinker,FileInputStream);
    FileInputStream getInputStream(DistributeLinker,uint64_t fileno);
    void removeReadLink(DistributeLinker,uint64_t fileno);

    uint64_t addWriteLink(DistributeLinker,FileOutputStream);
    FileOutputStream getOutputStream(DistributeLinker,uint64_t fileno);
    void removeWriteLink(DistributeLinker,uint64_t fileno);

    uint64_t addDownloadLink(DistributeLinker,File);
    File getDownloadFile(DistributeLinker,uint64_t);
    void removeDownloadFile(DistributeLinker,uint64_t);

    void addOpenFileLink(DistributeLinker,uint64_t fileno,File);
    File getOpenFile(DistributeLinker,uint64_t fileno);
    void removeOpenFile(DistributeLinker,uint64_t fileno);
    
    void onLinkerDisconnected(DistributeLinker);

    //sum function for Unit test
    size_t getReadLinkNums();
    size_t getWriteLinkNums();
    size_t getDownloadLinkNums();
    size_t getOpenLinkNums();

private:
    Mutex mStatusMutex;
    HashMap<String,HashMap<DistributeLinker,ArrayList<Integer>>> mStatus;
    uint8_t kActionConfimTable[Action::Max][Action::Max];

    //Linker to Files
    Mutex mFileMutex;
    HashMap<DistributeLinker,HashMap<Uint64,FileInputStream>> mReadLinks;
    HashMap<DistributeLinker,HashMap<Uint64,FileOutputStream>> mWriteLinks;
    HashMap<DistributeLinker,HashMap<Uint64,File>> mDownloadLinks;
    HashMap<DistributeLinker,HashMap<Uint64,File>> mOpenLinks;

    //file no generator
    AtomicUint64 mFileNoGenerator;
};

}

#endif