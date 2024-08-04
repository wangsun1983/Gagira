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
    Server server = Server::New();
    server->loadConfigFile("./abc.json");

    LoginController loginController = LoginController::New();
    ConversationController chatController = ConversationController::New();
    RegisterController regController = RegisterController::New();
    GroupController groupController = GroupController::New();

    TestController testController = TestController::New();
    printf("main trace1 \n");
    InjectController(st(HttpMethod)::Id::Get,"/login",loginController,login);
    InjectController(st(HttpMethod)::Id::Post,"/regist",regController,registUser);

    //InjectController(st(HttpMethod)::Get,"/testnewgroup",testController,testNewGroup);

    InjectController(st(HttpMethod)::Id::Post,"/chat",chatController,onNewConversation);

    InjectController(st(HttpMethod)::Id::Post,"/group/add",groupController,addNewGroup);
    
    printf("main trace2 \n");

    server->start();
    server->waitForExit();


    return 0;
}

