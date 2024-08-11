#include "ArchiveFileManager.hpp"
#include "AutoLock.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

const uint64_t _ArchiveFileManager::kInvalidFileNo = 0;
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
    mDownloadLinks = HashMap<DistributeLinker,HashMap<Uint64,File>>::New();
    mOpenLinks = HashMap<DistributeLinker,HashMap<Uint64,File>>::New();

    mFileNoGenerator = AtomicUint64::New(1); // 0:invalid
}


int _ArchiveFileManager::updateFileStatus(String path,DistributeLinker linker,Action action) {
    AutoLock l(mStatusMutex);
    //check whether path is exist
    File file = File::New(path);
    if(!file->exists()) {
        return -ENOENT;
    }
    auto status = mStatus->get(path);
    if(status != nullptr) {
        ForEveryOne(pair,status) {
            auto list = pair->getValue();
            ForEveryOne(current_action,list) {
                if(kActionConfimTable[current_action->toValue()][action] == 0) {
                    return -EBUSY;
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
        auto status = HashMap<DistributeLinker,ArrayList<Integer>>::New();
        auto actions = ArrayList<Integer>::New();
        actions->add(Integer::New(action));
        status->put(linker,actions);
        mStatus->put(path,status);
    }
    return 0;
}

void _ArchiveFileManager::removeFileStatus(String path,DistributeLinker linker,Action action) {
    AutoLock l(mStatusMutex);
    if(path == nullptr) {
        return;
    }
    
    auto status = mStatus->get(path);
    if(status != nullptr) {
        auto actionList = status->get(linker);
        if(actionList != nullptr) {
            actionList->remove(Integer::New(action));
            //no actions,remove linker
            if(actionList->size() == 0) {
                status->remove(linker);
            }
        }

        //no linker,remove all
        if(status->size() == 0) {
            mStatus->remove(path);
        }
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

        if(maps->size() == 0) {
            maps->remove(link);
        }
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
    maps->put(Uint64::New(fileno),output);
    return fileno;
}

FileOutputStream _ArchiveFileManager::getOutputStream(DistributeLinker link,uint64_t fileno) {
    AutoLock l(mFileMutex);
    auto maps = mWriteLinks->get(link);
    return maps == nullptr ? nullptr : maps->get(Uint64::New(fileno));
}

void _ArchiveFileManager::removeWriteLink(DistributeLinker link,uint64_t fileno) {
    AutoLock l(mFileMutex);
    auto maps = mWriteLinks->get(link);
    if(maps != nullptr) {
        maps->remove(Uint64::New(fileno));

        if(maps->size() == 0) {
            maps->remove(link);
        }
    }
}

uint64_t _ArchiveFileManager::addDownloadLink(DistributeLinker linker,File file) {
    AutoLock l(mFileMutex);
    auto fileno = mFileNoGenerator->getAndIncrement();;
    auto map = mDownloadLinks->get(linker);
    if(map == nullptr) {
        map = HashMap<Uint64,File>::New();
        mDownloadLinks->put(linker,map);
    }

    map->put(Uint64::New(fileno),file);
    return fileno;
}

File _ArchiveFileManager::getDownloadFile(DistributeLinker linker,uint64_t fileno) {
    AutoLock l(mFileMutex);
    auto map = mDownloadLinks->get(linker);
    if(map != nullptr) {
        return map->get(Uint64::New(fileno));
    }

    return nullptr;
}

void _ArchiveFileManager::removeDownloadFile(DistributeLinker linker,uint64_t fileno) {
    AutoLock l(mFileMutex);
    auto map = mDownloadLinks->get(linker);
    if(map != nullptr) {
        map->remove(Uint64::New(fileno));

        if(map->size() != 0) {
            mDownloadLinks->remove(linker);
        }
    }
}

void _ArchiveFileManager::addOpenFileLink(DistributeLinker linker,uint64_t fileno,File file) {
    AutoLock l(mFileMutex);
    auto map = mOpenLinks->get(linker);
    if(map == nullptr) {
        map = HashMap<Uint64,File>::New();
        mOpenLinks->put(linker,map);
    }

    map->put(Uint64::New(fileno),file);
}

File _ArchiveFileManager::getOpenFile(DistributeLinker linker,uint64_t fileno) {
    AutoLock l(mFileMutex);
    auto map = mOpenLinks->get(linker);
    if(map != nullptr) {
        return map->get(Uint64::New(fileno));
    }

    return nullptr;
}

void _ArchiveFileManager::removeOpenFile(DistributeLinker linker,uint64_t fileno) {
    AutoLock l(mFileMutex);
    auto map = mOpenLinks->get(linker);
    if(map != nullptr) {
        map->remove(Uint64::New(fileno));

        if(map->size() == 0) {
            mOpenLinks->remove(linker);
        }
    }
}

void _ArchiveFileManager::onLinkerDisconnected(DistributeLinker linker) {
    AutoLock l(mFileMutex);
    auto readers = mReadLinks->remove(linker);
    if(readers != nullptr) {
        ForEveryOne(pair,readers) {
            pair->getValue()->close();
        }
    }

    auto writers = mWriteLinks->remove(linker);
    if(writers != nullptr) {
        ForEveryOne(pair,writers) {
            pair->getValue()->close();
        }
    }

    mDownloadLinks->remove(linker);
    mOpenLinks->remove(linker);

    auto iterator = mStatus->getIterator();
    for(auto iterator = mStatus->getIterator();iterator->hasValue();iterator->next()) {
        auto map = iterator->getValue();
        map->remove(linker);
        if(map->size() == 0) {
            iterator->remove();
        }
    }
}

size_t _ArchiveFileManager::getReadLinkNums() {
    AutoLock l(mFileMutex);
    return mReadLinks->size();
}

size_t _ArchiveFileManager::getWriteLinkNums() {
    AutoLock l(mFileMutex);
    return mWriteLinks->size();
}

size_t _ArchiveFileManager::getDownloadLinkNums() {
    AutoLock l(mFileMutex);
    return mDownloadLinks->size();
}

size_t _ArchiveFileManager::getOpenLinkNums() {
    AutoLock l(mFileMutex);
    return mOpenLinks->size();
}

}

