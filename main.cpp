#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "Inet4Address.hpp"
#include "ServletRequest.hpp"
#include "HttpResourceManager.hpp"
#include "Reflect.hpp"
#include "Utils.hpp"
#include "Interceptor.hpp"

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

    HttpResponseEntity getStudent() {
        printf("sayhello function called \n");
        ServletRequest req = getRequest();
        //String s = req->getContent<String>();
        //printf("s is %s \n",s->toChars());
        Student s = req->getContent<Student>();
        printf("student is %s,age is %d \n",s->name->toChars(),s->age);
        //s->name = createString("aaaa");
        return createHttpResponseEntity(s);
    }
};

DECLARE_CLASS(MyInterceptor) IMPLEMENTS(Interceptor) {
public:
    bool onIntercept() {
        printf("onIntercept!!!!!!!!!!!!!! \n");
        return false;
    }
};

int main() {
    Server server = createServer()
                    ->setAddress(createInet4Address("192.168.1.10",1140));
    MyController controller = createMyController();
    
    HttpResourceManager resourceManager = st(HttpResourceManager)::getInstance();
    resourceManager->setResourceDir("./htm");
    resourceManager->setViewRedirect("index","index.html");

    InjectController(st(HttpMethod)::Get,"abc/:id",controller,sayHello);
    InjectController(st(HttpMethod)::Get,"student/:id",controller,getStudent);

    //InjectInterceptor(st(HttpMethod)::Get,"abc/:id",st(Interceptor)::BeforeExec,createMyInterceptor());
    //InjectGlobalInterceptor(st(HttpMethod)::Get,createMyInterceptor());
    //InjectResInterceptor("/index.html",createMyInterceptor());

    InjectController(st(HttpMethod)::Get,"abc/:id/:name",controller,sayHi);

    server->start();
    server->waitForExit();

    return 0;
}

