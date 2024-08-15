#ifndef __GAGRIA_FENCE_CONNECTION_HPP__
#define __GAGRIA_FENCE_CONNECTION_HPP__

#include "String.hpp"
#include "InetAddress.hpp"
#include "Socket.hpp"
#include "Mutex.hpp"
#include "DistributeMessageParser.hpp"
#include "DistributeMessageConverter.hpp"
#include "System.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(FenceConnection) {
public:
    _FenceConnection(String);
    ~_FenceConnection();

    int connect();
    int close();

    int acquireFence(String name,uint64_t waittime = 0);
    int acquireReadFence(String name,uint64_t waittime = 0);
    int acquireWriteFence(String name,uint64_t waittime = 0);

    int releaseFence(String name);
    int releaseReadFence(String name);
    int releaseWriteFence(String name);
    
private:
    enum {
        LocalFence = 0,
        LocalReadFence,
        LocalWriteFence
    };
    
    int acquireFence(int type,String name,uint64_t waittime);
    int releaseFence(int type,String name);

    InetAddress mAddress;
    Socket mSock;
    InputStream mInput;
    OutputStream mOutput;
    DistributeMessageConverter mConverter;
    DistributeMessageParser mParser;
    Mutex mutex;
    
    template<typename T>
    T waitResponse(InputStream input) {
        T resp = nullptr;
        auto parser = DistributeMessageParser::New(1024*16);
        ByteArray data = ByteArray::New(parser->getBufferSize());

        while(1) {
            int ret = input->read(data);
            if(ret == 0) {
                break;
            }
            data->quickShrink(ret);
            parser->pushData(data);
            auto response = parser->doParse();
            if(response != nullptr && response->size() > 0) {
                ByteArray parseData = response->get(0);
                resp = mConverter->generateMessage<T>(parseData);
                break;
            }
            data->quickRestore();
        }

        return resp;
    }
};

}
#endif
