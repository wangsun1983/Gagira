#ifndef __GAGRIA_DOCUMENT_CONNECTION_HPP__
#define __GAGRIA_DOCUMENT_CONNECTION_HPP__

#include "String.hpp"
#include "SocketListener.hpp"
#include "File.hpp"
#include "DistributeMessageParser.hpp"
#include "DistributeMessageConverter.hpp"
#include "InputStream.hpp"
#include "OutputStream.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(DocumentConnection) {
public:
    _DocumentConnection(String);
    ~_DocumentConnection();

    int upload(File);

    int download(String filename,String savepath);
    
    int connect();
    int close();

private:
    template<typename T>
    T waitResponse(InputStream input) {
        T resp = nullptr;
        auto parser = createDistributeMessageParser(1024*4);
        ByteArray data = createByteArray(parser->getBufferSize());

        while(1) {
            int ret = input->read(data);
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

    InetAddress mAddress;
    Socket mSock;
    InputStream mInput;
    OutputStream mOutput;
    DistributeMessageConverter mConverter;
};

}
#endif
