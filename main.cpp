#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "Inet4Address.hpp"
#include "ServletRequest.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_SIMPLE_CLASS(MyController) IMPLEMENTS(Controller) {
public:
    HttpResponseEntity sayHello(HashMap<String,String> m) {
        printf("sayhello function called \n");
        ServletRequest req = getRequest();
        printf("net ip is %s \n",req->getInetAddress()->toChars());

        return createHttpResponseEntity(123);
    }
};

int main() {
    Server server = createServer()
                    ->setAddress(createInet4Address("192.168.1.10",1124));
    MyController controller = createMyController();

    Inject(st(HttpMethod)::Get,"abc/:id",MyController,controller,sayHello);
    while(1) {
        sleep(100);
    }

    return 0;
}

