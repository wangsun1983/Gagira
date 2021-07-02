#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "Inet4Address.hpp"

using namespace obotcha;
using namespace gagira;

DECLARE_SIMPLE_CLASS(MyController) IMPLEMENTS(HttpController) {
public:
    HttpResponseEntity sayHello(HashMap<String,String> m) {
        printf("sayhello function called \n");

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

