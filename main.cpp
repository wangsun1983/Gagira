#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "Inet4Address.hpp"
#include "ServletRequest.hpp"
#include "HttpResourceManager.hpp"
#include "Reflect.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_SIMPLE_CLASS(Student) {
public:
    String name;
    int age;
    DECLARE_REFLECT_FIELD(Student,name,age)
};

DECLARE_SIMPLE_CLASS(MyController) IMPLEMENTS(Controller) {
public:
    HttpResponseEntity sayHello(HashMap<String,String> m) {
        printf("sayhello function called \n");
        ServletRequest req = getRequest();
        printf("net ip is %s \n",req->getInetAddress()->toChars());
        Student s = createStudent();
        s->name = "abc";
        s->age = 123;
        //s->name = createString("aaaa");
        return createHttpResponseEntity(s);
    }
};

int main() {
    Server server = createServer()
                    ->setAddress(createInet4Address("192.168.1.9",1128));
    MyController controller = createMyController();
    
    HttpResourceManager resourceManager = st(HttpResourceManager)::getInstance();
    resourceManager->setResourceDir("./htm");
    resourceManager->setViewRedirect("index","index.html");

    Inject(st(HttpMethod)::Get,"abc/:id",MyController,controller,sayHello);

    server->start();
    while(1) {
        sleep(100);
    }

    return 0;
}

