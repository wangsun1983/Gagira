#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "Inet4Address.hpp"
#include "ServletRequest.hpp"
#include "HttpResourceManager.hpp"
#include "Reflect.hpp"
#include "Utils.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_CLASS(Student) {
public:
    String name;
    int age;
    DECLARE_REFLECT_FIELD(Student,name,age)
};

DECLARE_CLASS(MyController) IMPLEMENTS(Controller) {
public:
    HttpResponseEntity sayHello() {
        printf("sayhello function called \n");
        ServletRequest req = getRequest();
        printf("net ip is %s \n",req->getInetAddress()->toChars());
        printf("id is %d \n",GetIntParam(id)->toValue());
        Student s = createStudent();
        s->name = "abc";
        s->age = 123;
        //s->name = createString("aaaa");
        return createHttpResponseEntity(s);
    }

    HttpResponseEntity sayHi() {
        printf("sayhello function called \n");
        ServletRequest req = getRequest();
        printf("net ip is %s \n",req->getInetAddress()->toChars());
        printf("id is %d \n",GetIntParam(id)->toValue());
        printf("name is %s \n",GetStringParam(name)->toChars());
        Student s = createStudent();
        s->name = "abc";
        s->age = 123;
        //s->name = createString("aaaa");
        return createHttpResponseEntity(s);
    }
};

int main() {
    Server server = createServer()
                    ->setAddress(createInet4Address("192.168.1.10",1128));
    MyController controller = createMyController();
    
    HttpResourceManager resourceManager = st(HttpResourceManager)::getInstance();
    resourceManager->setResourceDir("./htm");
    resourceManager->setViewRedirect("index","index.html");

    Inject(st(HttpMethod)::Get,"abc/:id",controller,sayHello);
    Inject(st(HttpMethod)::Get,"abc/:id/:name",controller,sayHi);

    server->start();
    server->waitForExit();

    return 0;
}

