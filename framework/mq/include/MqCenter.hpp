#ifndef __GAGRIA_MQ_CENTOR_HPP__
#define __GAGRIA_MQ_CENTOR_HPP__

#include "String.hpp"
#include "SocketMonitor.hpp"
#include "SocketListener.hpp"
#include "ReadWriteLock.hpp"
#include "InetAddress.hpp"
#include "ByteRingArray.hpp"
#include "ByteRingArrayReader.hpp"
#include "Condition.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MqCenter) IMPLEMENTS(SocketListener) {
public:
    _MqCenter(String url,int buffsize = 1024);

    void waitForExit(long interval = 0);

    int close();

private:
    static SocketMonitor monitor;

    void onSocketMessage(int,Socket,ByteArray);
    void dispatchMessage(Socket sock,ByteArray);

    InetAddress mAddrss;
    ServerSocket sock;

    ReadWriteLock mReadWriteLock;
    ReadLock mReadLock;
    WriteLock mWriteLock;
    HashMap<String,ArrayList<OutputStream>> mOutputStreams;

    ByteRingArray mBuffer;
    ByteRingArrayReader mReader;

    Mutex mMutex;
    HashMap<String,ByteArray> mStickyMessasge;

    Condition waitExit;

    int mCurrentMsgLen;
};

}

#endif
