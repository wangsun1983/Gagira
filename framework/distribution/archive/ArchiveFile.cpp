#include "ArchiveFile.hpp"

namespace gagira {

_ArchiveFile::_ArchiveFile(String path,String url) {
    mFileName = path;
    mRemoteUrl = url;
}

String _ArchiveFile::getFileName() {
    return mFileName;
}

String _ArchiveFile::getRemoteUrl() {
    return mRemoteUrl;
}

}

