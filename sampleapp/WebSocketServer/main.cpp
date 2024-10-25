#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "Inet4Address.hpp"
#include "ServletRequest.hpp"
#include "HttpResourceManager.hpp"
#include "Reflect.hpp"
#include "Utils.hpp"
#include "Serializable.hpp"
#include "CountDownLatch.hpp"
#include "WsResponseEntity.hpp"

using namespace obotcha;
using namespace gagira;

CountDownLatch latch = CountDownLatch::New(2);
bool isGetHit = false;
bool isCompleteHit = false;

DECLARE_CLASS(SimpleGetController) IMPLEMENTS(Controller) {
public:
    ResponseEntity get() {
        String value = GetWebsocketRequest()->toString();
        if(!value->sameAs("Hello, World")) {
            printf("testSimpleHttpGet case1 \n");
        }
        latch->countDown();
        isGetHit = true;
        return st(WsResponseEntity)::Create(String::New("response ack"));
    }

    ResponseEntity complete() {
        latch->countDown();
        isCompleteHit = true;
        ByteArray data = ByteArray::New(4);
        data[0] = 0x01;
        data[1] = 0x02;
        data[2] = 0x03;
        data[3] = 0x04;
        return st(WsResponseEntity)::Create(data);
    }
};

int main() {
    int port = 1234;
    Server server = Server::New();
    server->setWsAddress(Inet4Address::New(1234));
    
    SimpleGetController getController = SimpleGetController::New();

    server->start();
    InjectWsController("simpleget",getController,get);
    InjectWsController("complete",getController,complete);

    latch->await();
    server->close();
    
    printf("finished \n");
    return 0;
}
