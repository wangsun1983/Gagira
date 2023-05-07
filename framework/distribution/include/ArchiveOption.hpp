#ifndef __GAGRIA_ARCHIVE_OPTION_HPP__
#define __GAGRIA_ARCHIVE_OPTION_HPP__

#include "Object.hpp"
#include "String.hpp"
#include "DistributeOption.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ArchiveOption) IMPLEMENTS(DistributeOption) {
public:
    _ArchiveOption();

    _ArchiveOption *setSavedPath(String);
    _ArchiveOption *setMonitorPort(uint32_t start,uint32_t end);
    
    String getSavedPath();
    uint32_t getMonitorStartPort();
    uint32_t getMonitorEndPort();
    
private:
    static const String DefaultSavedPath;
    static const uint32_t DefaultStartPort;
    static const uint32_t DefaultEndPort;

    String mSavedPath;
    uint32_t mStartPort;
    uint32_t mEndPort;
};

}

#endif
