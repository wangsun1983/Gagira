#include "ArchiveFileManager.hpp"
#include "AutoLock.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

//------- ArchiveFileManager --------
_ArchiveFileManager::_ArchiveFileManager() {
    mStatus = HashMap<String,HashMap<DistributeLinker,ArrayList<Integer>>>::New();
    mStatusMutex = Mutex::New();
    /*
                    | ----------------  current status  ----------------------|
                    Upload   Download   Delete  Rename  Copy  Write  Read  Move   
        Upload          X        X        X       X      X     X      X     X
        Download        X        Y        X       X      Y     X      Y     X 
        Delete          X        X        Y       X      X     X      X     X
        Rename          X        X        X       X      X     X      X     X
        Copy            X        Y        X       X      Y     X      Y     X
        Write           X        X        X       X      X     X      X     X
        Read            X        Y        X       X      Y     X      Y     X
        Move            X        X        X       X      X     X      X     X
    */
    //kActionConfimTable[current_status][confirm_status]
    memset(kActionConfimTable,0,sizeof(uint8_t)*Action::Max*Action::Max);
    //download
    kActionConfimTable[Action::Download][Action::Download] = 1;
    kActionConfimTable[Action::Download][Action::Copy] = 1;
    kActionConfimTable[Action::Download][Action::Read] = 1;

    //delete
    kActionConfimTable[Action::Delete][Action::Delete] = 1;

    //copy
    kActionConfimTable[Action::Copy][Action::Download] = 1;
    kActionConfimTable[Action::Copy][Action::Copy] = 1;
    kActionConfimTable[Action::Copy][Action::Read] = 1;

    //read
    kActionConfimTable[Action::Read][Action::Download] = 1;
    kActionConfimTable[Action::Read][Action::Copy] = 1;
    kActionConfimTable[Action::Read][Action::Read] = 1;

    //create all links
    mFileMutex = Mutex::New();
    mReadLinks = HashMap<DistributeLinker,HashMap<Uint64,FileInputStream>>::New();
    mWriteLinks = HashMap<DistributeLinker,HashMap<Uint64,FileOutputStream>>::New();
    mDownloadLinks = HashMap<Uint64,File>::New();

    mFileNoGenerator = AtomicUint64::New(1); // 0:error
}


int _ArchiveFileManager::updateFileStatus(String path,DistributeLinker linker,Action action) {
    AutoLock l(mStatusMutex);
    printf("updateFileStatus trace1 \n");
    auto status = mStatus->get(path);
    if(status != nullptr) {
        printf("updateFileStatus trace2 \n");
        ForEveryOne(pair,status) {
            auto list = pair->getValue();
            ForEveryOne(current_action,list) {
                printf("updateFileStatus,path is %s,current_action is %d,action is %d \n",path->toChars(),current_action->toValue(),action);
                if(kActionConfimTable[current_action->toValue()][action] == 0) {
                    return -1;
                }
            }
        }
        //check all action is ok
        auto selfList = status->get(linker);
        if(selfList == nullptr) {
            selfList = ArrayList<Integer>::New();
            status->put(linker,selfList);
        }
        selfList->add(Integer::New(action));
    } else {
        printf("updateFileStatus trace3 \n");
        auto status = HashMap<DistributeLinker,ArrayList<Integer>>::New();
        auto actions = ArrayList<Integer>::New();
        actions->add(Integer::New(action));
        status->put(linker,actions);
        mStatus->put(path,status);
        printf("updateFileStatus trace4,add path is %s \n",path->toChars());
    }
    return 0;
}

void _ArchiveFileManager::removeFileStatus(String path,DistributeLinker linker,Action action) {
    AutoLock l(mStatusMutex);
    printf("ArchiveFileManager removeFileStatus trace1,action is %d,path is %s \n",action,path->toChars());
    auto status = mStatus->get(path);
    auto actionList = status->get(linker);
    printf("ArchiveFileManager removeFileStatus trace2,action list size is %d\n",actionList->size());
    actionList->remove(Integer::New(action));

    //no actions,remove linker
    if(actionList->size() == 0) {
        status->remove(linker);
    }
}

bool _ArchiveFileManager::isFileOwner(String path,DistributeLinker linker) {
    //TODO
    return 0;
}

uint64_t _ArchiveFileManager::addReadLink(DistributeLinker link,FileInputStream input) {
    AutoLock l(mFileMutex);
    auto maps = mReadLinks->get(link);
    auto fileno = mFileNoGenerator->getAndIncrement();
    if(maps == nullptr) {
        maps = HashMap<Uint64,FileInputStream>::New();
        mReadLinks->put(link,maps);
    }

    maps->put(Uint64::New(fileno),input);
    return fileno;
}

FileInputStream _ArchiveFileManager::getInputStream(DistributeLinker link,uint64_t fileno) {
    AutoLock l(mFileMutex);
    auto maps = mReadLinks->get(link);
    return maps == nullptr ? nullptr : maps->get(Uint64::New(fileno));
}

void _ArchiveFileManager::removeReadLink(DistributeLinker link,uint64_t fileno) {
    AutoLock l(mFileMutex);
    auto maps = mReadLinks->get(link);
    if(maps != nullptr) {
        maps->remove(Uint64::New(fileno));
    }
}

uint64_t _ArchiveFileManager::addWriteLink(DistributeLinker link,FileOutputStream output) {
    AutoLock l(mFileMutex);
    auto maps = mWriteLinks->get(link);
    auto fileno = mFileNoGenerator->getAndIncrement();

    if(maps == nullptr) {
        maps = HashMap<Uint64,FileOutputStream>::New();
        mWriteLinks->put(link,maps);
    }
    printf("ArchiveFileManager add write link fileno is %d,link is %lx",fileno,link.get_pointer());
    maps->put(Uint64::New(fileno),output);
    return fileno;
}

FileOutputStream _ArchiveFileManager::getOutputStream(DistributeLinker link,uint64_t fileno) {
    AutoLock l(mFileMutex);
    auto maps = mWriteLinks->get(link);
    printf("ArchiveFileManager get write link fileno is %d,link is %lx",fileno,link.get_pointer());
    return maps == nullptr ? nullptr : maps->get(Uint64::New(fileno));
}

void _ArchiveFileManager::removeWriteLink(DistributeLinker link,uint64_t fileno) {
    AutoLock l(mFileMutex);
    auto maps = mWriteLinks->get(link);
    if(maps != nullptr) {
        maps->remove(Uint64::New(fileno));
    }
}

uint64_t _ArchiveFileManager::addDownloadLink(File file) {
    AutoLock l(mFileMutex);
    auto fileno = mFileNoGenerator->getAndIncrement();;
    mDownloadLinks->put(Uint64::New(fileno),file);
    return fileno;
}

File _ArchiveFileManager::getDownloadFile(uint64_t fileno) {
    AutoLock l(mFileMutex);
    return mDownloadLinks->get(Uint64::New(fileno));
}

void _ArchiveFileManager::removeDownloadFile(uint64_t fileno) {
    AutoLock l(mFileMutex);
    mDownloadLinks->remove(Uint64::New(fileno));
}

void _ArchiveFileManager::addOpenFileLink(uint64_t fileno,File file) {
    AutoLock l(mFileMutex);
    mDownloadLinks->put(Uint64::New(fileno),file);
}

File _ArchiveFileManager::getOpenFile(uint64_t fileno) {
    AutoLock l(mFileMutex);
    return mDownloadLinks->get(Uint64::New(fileno));
}

void _ArchiveFileManager::removeOpenFile(uint64_t fileno) {
    AutoLock l(mFileMutex);
    mDownloadLinks->remove(Uint64::New(fileno));
}

}

