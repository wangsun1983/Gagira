#ifndef __GAGRIA_ARCHIVE_CONNECTION_HPP__
#define __GAGRIA_ARCHIVE_CONNECTION_HPP__

#include "String.hpp"
#include "SocketListener.hpp"
#include "File.hpp"
#include "DistributeMessageParser.hpp"
#include "DistributeMessageConverter.hpp"
#include "InputStream.hpp"
#include "OutputStream.hpp"
#include "ArchiveMessage.hpp"

using namespace obotcha;

namespace gagira {

typedef void (*onUploadStatus)(int status);

DECLARE_CLASS(ArchiveConnection) {
public:
    enum UploadStatus {
        FinishSendReq = 0,
        StartUploading = 1,
        WaitResponse = 2,
    };

    _ArchiveConnection(String);
    ~_ArchiveConnection();

    int upload(File,onUploadStatus func = nullptr);
    int download(String filename,String savepath);
    int remove(String filename);
    int rename(String originalname,String newname);
    uint64_t querySize(String filename);
    
    int connect();
    int close();
    int getErr();

    //return value(fileno,result)
    DefRet(uint64_t,int) openStream(String filename,uint64_t flags);
    ByteArray read(uint64_t fileno,uint64_t length);
    int seekTo(uint64_t fileno,uint32_t pos,st(ApplySeekToMessage)::Type type);
    int write(uint64_t fileno,ByteArray);
    int closeStream(uint64_t fileno);
private:
    template<typename T>
    T waitResponse(InputStream input) {
        T resp = nullptr;
        auto parser = DistributeMessageParser::New(1024*32);
        ByteArray data = ByteArray::New(parser->getBufferSize());

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
    Mutex mMutex;
    int mErrno;
};

}
#endif
