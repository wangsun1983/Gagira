#ifndef __GAGRIA_DOCUMENT_LINKER_HPP__
#define __GAGRIA_DOCUMENT_LINKER_HPP__

#include "Object.hpp"
#include "DistributeLinker.hpp"
#include "FileOutputStream.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DocumentLinker) IMPLEMENTS (DistributeLinker) {
public:
    enum Status {
        Idle = 0,
        WaitClientApplyInfo,
        WaitClientMessage,
    };

    _DocumentLinker(Socket,int buffsize = 1024*32);
    _DocumentLinker * setFileSize(uint64_t);
    _DocumentLinker * setPath(String path);

    uint64_t reduceFileSize(uint32_t length);
    void setStatus(int);
    int getStatus();

    OutputStream getOutputStream();
    ~_DocumentLinker();
private:
    OutputStream mOutputStream;
    uint64_t mFileSize;
    String mPath;
    int mStatus;
};

}

#endif
