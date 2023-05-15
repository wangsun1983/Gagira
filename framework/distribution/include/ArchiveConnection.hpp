#ifndef __GAGRIA_ARCHIVE_CONNECTION_HPP__
#define __GAGRIA_ARCHIVE_CONNECTION_HPP__

#include "String.hpp"
#include "SocketListener.hpp"
#include "File.hpp"
#include "DistributeMessageParser.hpp"
#include "DistributeMessageConverter.hpp"
#include "InputStream.hpp"
#include "OutputStream.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(ArchiveConnection) {
public:
    _ArchiveConnection(String);
    ~_ArchiveConnection();

    int upload(File);
    int download(String filename,String savepath);
    uint64_t querySize(String filename);
    int open(String filename,uint64_t flags);
    ByteArray read(uint64_t length);
    int seekTo(uint32_t pos);
    int write(ByteArray);
    int rename(String originalname,String newname);
    int delFile(String filename);
    
    int connect();
    int close();

private:
    template<typename T>
    T waitResponse(InputStream input) {
        T resp = nullptr;
        auto parser = createDistributeMessageParser(1024*16);
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
