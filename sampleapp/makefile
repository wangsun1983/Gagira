appDir = sampleapp

cflags += -I ./sampleapp/include/ \
		  -I ./sampleapp/datastruct/ 

objs += $(outtmp)/main.o \
		$(outtmp)/userinfodao.o \
		$(outtmp)/groupinfodao.o \
		$(outtmp)/conversationdao.o \
		$(outtmp)/userservice.o \
		$(outtmp)/groupservice.o \
		$(outtmp)/conversationservice.o \
		$(outtmp)/onlineservice.o \
		$(outtmp)/logincontroller.o \
		$(outtmp)/conversationcontroller.o \
		$(outtmp)/registercontroller.o \
		$(outtmp)/clientwscontroller.o \
		$(outtmp)/groupcontroller.o \
		$(outtmp)/testcontroller.o \
		$(outtmp)/sessionidgenerator.o 
		

$(outtmp)/main.o :$(appDir)/main.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/main.cpp -o $(outtmp)/main.o

$(outtmp)/userinfodao.o :$(appDir)/dao/UserInfoDao.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/dao/UserInfoDao.cpp -o $(outtmp)/userinfodao.o

$(outtmp)/groupinfodao.o :$(appDir)/dao/GroupInfoDao.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/dao/GroupInfoDao.cpp -o $(outtmp)/groupinfodao.o

$(outtmp)/conversationdao.o :$(appDir)/dao/ConversationDao.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/dao/ConversationDao.cpp -o $(outtmp)/conversationdao.o

$(outtmp)/userservice.o :$(appDir)/service/UserService.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/service/UserService.cpp -o $(outtmp)/userservice.o

$(outtmp)/groupservice.o :$(appDir)/service/GroupService.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/service/GroupService.cpp -o $(outtmp)/groupservice.o

$(outtmp)/conversationservice.o :$(appDir)/service/ConversationService.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/service/ConversationService.cpp -o $(outtmp)/conversationservice.o

$(outtmp)/onlineservice.o :$(appDir)/service/OnlineService.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/service/OnlineService.cpp -o $(outtmp)/onlineservice.o

$(outtmp)/logincontroller.o :$(appDir)/controller/LoginController.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/controller/LoginController.cpp -o $(outtmp)/logincontroller.o

$(outtmp)/conversationcontroller.o :$(appDir)/controller/ConversationController.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/controller/ConversationController.cpp -o $(outtmp)/conversationcontroller.o

$(outtmp)/registercontroller.o :$(appDir)/controller/RegisterController.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/controller/RegisterController.cpp -o $(outtmp)/registercontroller.o

$(outtmp)/clientwscontroller.o :$(appDir)/controller/ClientWsController.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/controller/ClientWsController.cpp -o $(outtmp)/clientwscontroller.o

$(outtmp)/groupcontroller.o :$(appDir)/controller/GroupController.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/controller/GroupController.cpp -o $(outtmp)/groupcontroller.o

$(outtmp)/sessionidgenerator.o :$(appDir)/SessionIdGenerator.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/SessionIdGenerator.cpp -o $(outtmp)/sessionidgenerator.o

$(outtmp)/testcontroller.o :$(appDir)/controller/TestController.cpp
	$(cppcompiler) $(gagiracppflags) -c $(appDir)/controller/TestController.cpp -o $(outtmp)/testcontroller.o
