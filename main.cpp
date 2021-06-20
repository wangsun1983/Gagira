#include <stdio.h>
#include <unistd.h>

#include "HttpServer.hpp"
#include "HttpServerBuilder.hpp"
#include "Inet4Address.hpp"

using namespace obotcha;

String htmlpath = "./static/homepage_qdrs/src/main/resources/templates/";
String resourcepath = "./static/homepage_qdrs/src/main/resources/";
String fontpath = "./static/homepage_qdrs/src/main/resources/static/fonts";
String imgpath = "./static/homepage_qdrs/src/main/resources/static/fimgonts";
String jspath = "./static/homepage_qdrs/src/main/resources/static/js";

DECLARE_SIMPLE_CLASS(MyHttpListener) IMPLEMENTS(HttpListener) {

public:
    void onHttpMessage(int event,HttpLinker client,HttpResponseWriter w,HttpPacket msg) {
        switch(event) {
            case Message:
            printf("message coming \n");
            HttpEntity entity = msg->getEntity();
            HttpHeader header = msg->getHeader();
            HttpUrl url = header->getUrl();
            
            String path = url->getPath();
            String dir = nullptr;

            if(path->indexOf(".html") > 0) {
                dir = htmlpath;
            } else {
                dir = resourcepath;
            }
            HttpResponse response = createHttpResponse();
            if(dir != nullptr) {
                String filepath = dir->append(url->getPath());
                printf("path is %s \n",filepath->toChars());

                File f = createFile(filepath);
                if(f->exists()) {
                    printf("file exists \n");
                } else {
                    printf("file not exists \n");
                }
                response->setFile(f); 
            }            
            w->write(response);

            break;
        }
    }
};

int main() {
    MyHttpListener listener = createMyHttpListener();
    HttpServer server = createHttpServerBuilder()
                    ->setAddress(createInet4Address(1212))
                    ->setListener(listener)
                    ->build();
    server->start();
    while(1) {
        sleep(100);
    }
}

