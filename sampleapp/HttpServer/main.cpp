#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "Inet4Address.hpp"
#include "ServletRequest.hpp"
#include "HttpResourceManager.hpp"
#include "Reflect.hpp"
#include "Aspect.hpp"
#include "Serializable.hpp"
#include "CountDownLatch.hpp"
#include "Reflect.hpp"

using namespace obotcha;
using namespace gagira;

CountDownLatch latch = CountDownLatch::New(2);
bool isGetHit = false;
bool isGet2Hit = false;

DECLARE_CLASS(SimpleData) {
public:
    int value;
    String name;
    
    DECLARE_REFLECT_FIELD(SimpleData,value,name)
};

DECLARE_CLASS(SimpleGetController) IMPLEMENTS(Controller) {
public:
    ResponseEntity get() {
        String value = GetStringParam(value);
        if(!value->sameAs("100")) {
            printf("testSimpleHttpGet case1 \n");
        }
        isGetHit = true;
        latch->countDown();
        SimpleData data = SimpleData::New();
        data->value = 123;
        data->name = String::New("hh");
        return st(HttpResponseEntity)::Create(data);
    }

    ResponseEntity get2() {
        String value = GetStringParam(value);
        if(!value->sameAs("200")) {
            printf("testSimpleHttpGet case2 \n");
        }
        isGet2Hit = true;
        latch->countDown();
        return nullptr;
    }
};

int main() {
    int port = 1234;
    Server server = Server::New();
    server->setAddress(Inet4Address::New(port));
    server->start();
    SimpleGetController getController = SimpleGetController::New();

    InjectController(st(HttpMethod)::Id::Get,"/simpleget/{value}",getController,get);
    InjectController(st(HttpMethod)::Id::Get,"/simpleget2/{value}",getController,get2);

    latch->await();
    server->close();
    
    printf("finish \n");
    return 0;
}
