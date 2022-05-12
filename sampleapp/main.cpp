#include <stdio.h>
#include <unistd.h>

#include "Controller.hpp"
#include "Server.hpp"
#include "LoginController.hpp"
#include "ConversationController.hpp"
#include "RegisterController.hpp"
#include "TestController.hpp"
#include "Utils.hpp"
#include "GroupController.hpp"

using namespace obotcha;
using namespace gagira;


int main() {
    Server server = createServer();
    server->setConfigFile("./abc.json");

    LoginController loginController = createLoginController();
    ConversationController chatController = createConversationController();
    RegisterController regController = createRegisterController();
    GroupController groupController = createGroupController();

    TestController testController = createTestController();
    printf("main trace1 \n");
    InjectController(st(HttpMethod)::Get,"/login",loginController,login);
    InjectController(st(HttpMethod)::Post,"/regist",regController,registUser);

    //InjectController(st(HttpMethod)::Get,"/testnewgroup",testController,testNewGroup);

    InjectController(st(HttpMethod)::Post,"/chat",chatController,onNewConversation);

    InjectController(st(HttpMethod)::Post,"/group/add",groupController,addNewGroup);
    
    printf("main trace2 \n");

    server->start();
    server->waitForExit();


    return 0;
}

