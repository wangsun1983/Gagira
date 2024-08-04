#ifndef __GAGRIA_ARCHIVE_FILE_HPP__
#define __GAGRIA_ARCHIVE_FILE_HPP__

#include "Object.hpp"
#include "ArchiveConnection.hpp"
#include "File.hpp"

using namespace obotcha;

namespace gagira {

class _ArchiveFileInputStream;
class _ArchiveFileOutputStream;

DECLARE_CLASS(ArchiveFile) {
public:
    friend class _ArchiveFileInputStream;
    friend class _ArchiveFileOutputStream;

    _ArchiveFile(String filename,String url);
    String getFileName();
    String getRemoteUrl();

private:
    String mFileName;
    String mRemoteUrl;
};

}

#endif
