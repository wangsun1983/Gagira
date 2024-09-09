#ifndef __GAGRIA_ARCHIVE_CLIENT_HPP__
#define __GAGRIA_ARCHIVE_CLIENT_HPP__

#include "Object.hpp"
#include "ArchiveConnection.hpp"
#include "ArchiveInputStream.hpp"
#include "ArchiveOutputStream.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ArchiveFactory) {
public:
    _ArchiveFactory(String);
    //int connect();
    ArchiveInputStream openRead(String filename);
    ArchiveOutputStream openTruncWrite(String filename);
    ArchiveOutputStream openAppendWrite(String filename);

    int upload(File);
    int download(String filename,String savepath);
    int remove(String filename);
    int rename(String originalname,String newname);
    uint64_t querySize(String filename);
private:
    ArchiveConnection mConnection;
};


}

#endif
