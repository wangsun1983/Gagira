#include "FenceConnection.hpp"
#include "InitializeException.hpp"
#include "HttpUrl.hpp"
#include "FenceMessage.hpp"

using namespace obotcha;

namespace gagira {

_FenceConnection::_FenceConnection(String s) {
    HttpUrl url = createHttpUrl(s);
    mAddress = url->getInetAddress()->get(0);
    if(mAddress == nullptr) {
        Trigger(InitializeException,"Failed to find MqCenter");
    }
    mParser = createDistributeMessageParser(1024*4);
    mConverter = createDistributeMessageConverter();
}

_FenceConnection::~_FenceConnection() {
    close();
}

int _FenceConnection::connect() {
    mSock = createSocketBuilder()->setAddress(mAddress)->newSocket();
    Inspect(mSock->connect() < 0,-1);

    mInput = mSock->getInputStream();
    mOutput = mSock->getOutputStream();
    return 0;
}

int _FenceConnection::close() {
    if(mSock != nullptr) {
        mSock->close();
        mSock = nullptr;
    }
    return 0;
}

int _FenceConnection::acquireFence(String name,uint64_t waittime) {
    return acquireFence(LocalFence,name,waittime);
}

int _FenceConnection::acquireReadFence(String name,uint64_t waittime) {
    return acquireFence(LocalReadFence,name,waittime);
}

int _FenceConnection::acquireWriteFence(String name,uint64_t waittime) {
    return acquireFence(LocalWriteFence,name,waittime);
}

int _FenceConnection::acquireFence(int type,String name,uint64_t waittime) {
    FenceMessage msg = nullptr;
    switch(type) {
        case LocalFence:
            msg = createFenceMessage(st(FenceMessage)::ApplyFence,name,waittime);
            break;

        case LocalReadFence:
            msg = createFenceMessage(st(FenceMessage)::ApplyReadFence,name,waittime);
            break;

        case LocalWriteFence:
            msg = createFenceMessage(st(FenceMessage)::ApplyWriteFence,name,waittime);
            break;
    }
    
    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -ENETUNREACH;
    }

    auto resp = waitResponse<ConfirmFenceMessage>(mInput);
    return -resp->getResult();
}


int _FenceConnection::releaseFence(String name) {
    return releaseFence(LocalFence,name);
}

int _FenceConnection::releaseReadFence(String name) {
    return releaseFence(LocalReadFence,name);
}

int _FenceConnection::releaseWriteFence(String name) {
    return releaseFence(LocalWriteFence,name);
}

int _FenceConnection::releaseFence(int type,String name) {
    FenceMessage msg = nullptr;
    switch(type) {
        case LocalFence:
            msg = createFenceMessage(st(FenceMessage)::ApplyReleaseFence,name);
            break;

        case LocalReadFence:
            msg = createFenceMessage(st(FenceMessage)::ApplyReleaseReadFence,name);
            break;

        case LocalWriteFence:
            msg = createFenceMessage(st(FenceMessage)::ApplyReleaseWriteFence,name);
            break;
    }

    if(mOutput->write(mConverter->generatePacket(msg)) < 0) {
        return -ENETUNREACH;
    }
    
    auto resp = waitResponse<ConfirmFenceMessage>(mInput);
    return -resp->getResult();
}

}
