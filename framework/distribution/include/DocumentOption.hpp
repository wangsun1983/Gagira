#ifndef __GAGRIA_DOCUMENT_OPTION_HPP__
#define __GAGRIA_DOCUMENT_OPTION_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "DistributeOption.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DocumentHandler) {
public:
    virtual String onUploadFile(String filename) = 0;
    virtual String onDownloadFile(String filename) = 0;
};

DECLARE_CLASS(DocumentOption) IMPLEMENTS(DistributeOption) {
public:
    _DocumentOption();

    _DocumentOption *setSavedPath(String);
    _DocumentOption *setMonitorPort(uint32_t start,uint32_t end);
    _DocumentOption *setHandler(DocumentHandler);

    String getSavedPath();
    uint32_t getMonitorStartPort();
    uint32_t getMonitorEndPort();
    DocumentHandler getHandler();
    
private:
    static const String DefaultSavedPath;
    static const uint32_t DefaultStartPort;
    static const uint32_t DefaultEndPort;

    String mSavedPath;
    uint32_t mStartPort;
    uint32_t mEndPort;
    DocumentHandler mHandler;
};

}

#endif
