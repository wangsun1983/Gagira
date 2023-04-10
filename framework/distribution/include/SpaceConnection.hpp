#ifndef __GAGRIA_SPACE_CONNECTION_HPP__
#define __GAGRIA_SPACE_CONNECTION_HPP__

#include "String.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "SocketMonitor.hpp"
#include "MqMessage.hpp"
#include "Mutex.hpp"
#include "ByteRingArray.hpp"
#include "ByteRingArrayReader.hpp"
#include "DistributeMessageParser.hpp"
#include "DistributeMessageConverter.hpp"
#include "SpaceMessage.hpp"
#include "ReadWriteLock.hpp"
#include "System.hpp"
#include "SocketBuilder.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(SpaceConnectionListener) {
public:
    virtual void onConnect() = 0;
    virtual void onDisconnect() = 0;
    virtual void onDataUpdate(String tag,ByteArray data) = 0;
};

DECLARE_CLASS(SpaceConnection) IMPLEMENTS(SocketListener) {
public:
    _SpaceConnection(String,SpaceConnectionListener l = nullptr);
    ~_SpaceConnection();

    template<typename T>
    int update(String tag,T value) {
        auto message = createSpaceUpdateMessage(tag,value->serialize(),nullptr);
        return mOutput->write(mConverter->generatePacket(message));
    }

    int remove(String tag);

    int monitor(ArrayList<String>);
    int monitor(String);
    int unMonitor(ArrayList<String>);
    int unMonitor(String);

    template< typename T>
    T get(String tag) {
        auto message = createSpaceAcquireMessage(tag);
        DistributeMessageParser parser = createDistributeMessageParser(32*1024);
        auto sock = createSocketBuilder()->setAddress(mAddress)->newSocket();
        if(sock->connect() < 0) {
            return nullptr;
        }

        if(sock->getOutputStream()->write(mConverter->generatePacket(message)) < 0) {
            return nullptr;
        }

        ArrayList<ByteArray> response = nullptr;
        while(1) {
            ByteArray data = createByteArray(4*1024);
            int ret = sock->getInputStream()->read(data);
            if(ret > 0) {
                data->quickShrink(ret);
                parser->pushData(data);
                response = parser->doParse();
                if(response == nullptr || response->size() == 0) {
                    continue;
                }
            }
            break;
        }

        sock->close();
        if(response != nullptr && response->size() != 0) {
            auto msg = mConverter->generateMessage<SpaceMessageResult>(response->get(0));
            T ret = AutoCreate<T>();
            ret->deserialize(msg->data);
            return ret;
        }
        
        return nullptr;
    }

    int connect();
    int close();

private:
    InetAddress mAddress;
    Socket mSock;
    InputStream mInput;
    OutputStream mOutput;
    DistributeMessageConverter mConverter;
    DistributeMessageParser mParser;
    SpaceConnectionListener mListener;

    SocketMonitor mSocketMonitor;
    void onSocketMessage(int,Socket,ByteArray);
};

}
#endif
