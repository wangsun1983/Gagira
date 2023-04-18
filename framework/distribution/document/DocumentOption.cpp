#include "DocumentOption.hpp"

using namespace obotcha;

namespace gagira {

const String _DocumentOption::DefaultSavedPath = createString("./");
const uint32_t _DocumentOption::DefaultStartPort = 1200;
const uint32_t _DocumentOption::DefaultEndPort = 1300;

_DocumentOption::_DocumentOption():_DistributeOption() {
    mSavedPath = DefaultSavedPath;
    mStartPort = DefaultStartPort;
    mEndPort = DefaultEndPort;
}

_DocumentOption *_DocumentOption::setSavedPath(String path) {
    mSavedPath = path;
    return this;
}

_DocumentOption *_DocumentOption::setMonitorPort(uint32_t start,uint32_t end) {
    mStartPort = start;
    mEndPort = end;
    return this;
}

_DocumentOption *_DocumentOption::setHandler(DocumentHandler h) {
    mHandler = h;
    return this;
}

String _DocumentOption::getSavedPath() {
    return mSavedPath;
}

uint32_t _DocumentOption::getMonitorStartPort() {
    return mStartPort;
}

uint32_t _DocumentOption::getMonitorEndPort() {
    return mEndPort;
}

DocumentHandler _DocumentOption::getHandler() {
    return mHandler;
}

}

