#include "ArchiveOption.hpp"

using namespace obotcha;

namespace gagira {

const String _ArchiveOption::DefaultSavedPath = createString("./");
const uint32_t _ArchiveOption::DefaultStartPort = 1200;
const uint32_t _ArchiveOption::DefaultEndPort = 1300;

_ArchiveOption::_ArchiveOption():_DistributeOption() {
    mSavedPath = DefaultSavedPath;
    mStartPort = DefaultStartPort;
    mEndPort = DefaultEndPort;
}

_ArchiveOption *_ArchiveOption::setSavedPath(String path) {
    mSavedPath = path;
    return this;
}

_ArchiveOption *_ArchiveOption::setMonitorPort(uint32_t start,uint32_t end) {
    mStartPort = start;
    mEndPort = end;
    return this;
}

String _ArchiveOption::getSavedPath() {
    return mSavedPath;
}

uint32_t _ArchiveOption::getMonitorStartPort() {
    return mStartPort;
}

uint32_t _ArchiveOption::getMonitorEndPort() {
    return mEndPort;
}

}

