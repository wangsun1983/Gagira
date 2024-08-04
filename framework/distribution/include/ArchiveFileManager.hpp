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

    uint64_t addDownloadLink(File);
    File getDownloadFile(uint64_t);
    void removeDownloadFile(uint64_t);

    void addOpenFileLink(uint64_t fileno,File);
    File getOpenFile(uint64_t fileno);
    void removeOpenFile(uint64_t fileno);

private:
    Mutex mStatusMutex;
    HashMap<String,HashMap<DistributeLinker,ArrayList<Integer>>> mStatus;
    uint8_t kActionConfimTable[Action::Max][Action::Max];

    //Linker to Files
    Mutex mFileMutex;
    HashMap<DistributeLinker,HashMap<Uint64,FileInputStream>> mReadLinks;
    HashMap<DistributeLinker,HashMap<Uint64,FileOutputStream>> mWriteLinks;
    HashMap<Uint64,File> mDownloadLinks;

    //file no generator
    AtomicUint64 mFileNoGenerator;
};

}

#endif