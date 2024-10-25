#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include "InetAddress.hpp"
#include "Inet4Address.hpp"

using namespace obotcha;
using namespace gagira;


int main() {
    Server server = Server::New();
    //server->loadConfigFile("./abc.json");
    auto address = Inet4Address::New("192.168.137.124",8086);
    server->setAddress(address);
    
    InjectResource("./html/webhtml/src/main/resources/templates");
    InjectResource("./html/webhtml/src/main/resources/static");

    InjectDefaultHomePage("test_index.html");
    InjectViewRedirect("second","second.html");
    InjectViewRedirect("thrid","third.html");

    server->start();
    server->waitForExit();


    return 0;
}

