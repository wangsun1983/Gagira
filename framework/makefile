frameworkDir = ./framework

include ./framework/config/makefile
include ./framework/dao/makefile
include ./framework/template/makefile
include ./framework/distribution/makefile

objs += $(outtmp)/controller.o \
		$(outtmp)/server.o \
		$(outtmp)/servletrequest.o \
		$(outtmp)/globalcachemanager.o \
		$(outtmp)/controllerparam.o \
		$(outtmp)/httproutermanager.o \
		$(outtmp)/websocketroutermanager.o \
		$(outtmp)/httproutermap.o \
		$(outtmp)/httpresourcemanager.o \
		$(outtmp)/httpresponseentity.o \
		$(outtmp)/httprouter.o 

$(outtmp)/httproutermanager.o :$(frameworkDir)/HttpRouterManager.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/HttpRouterManager.cpp -o $(outtmp)/httproutermanager.o

$(outtmp)/websocketroutermanager.o :$(frameworkDir)/WebSocketRouterManager.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/WebSocketRouterManager.cpp -o $(outtmp)/websocketroutermanager.o

$(outtmp)/httproutermap.o :$(frameworkDir)/HttpRouterMap.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/HttpRouterMap.cpp -o $(outtmp)/httproutermap.o

$(outtmp)/httpresourcemanager.o :$(frameworkDir)/HttpResourceManager.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/HttpResourceManager.cpp -o $(outtmp)/httpresourcemanager.o

$(outtmp)/httpresponseentity.o :$(frameworkDir)/HttpResponseEntity.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/HttpResponseEntity.cpp -o $(outtmp)/httpresponseentity.o

$(outtmp)/httprouter.o :$(frameworkDir)/HttpRouter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/HttpRouter.cpp -o $(outtmp)/httprouter.o

$(outtmp)/controller.o :$(frameworkDir)/Controller.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/Controller.cpp -o $(outtmp)/controller.o

$(outtmp)/controllerparam.o :$(frameworkDir)/ControllerParam.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/ControllerParam.cpp -o $(outtmp)/controllerparam.o

$(outtmp)/server.o :$(frameworkDir)/Server.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/Server.cpp -o $(outtmp)/server.o

$(outtmp)/servletrequest.o :$(frameworkDir)/ServletRequest.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/ServletRequest.cpp -o $(outtmp)/servletrequest.o

$(outtmp)/globalcachemanager.o :$(frameworkDir)/GlobalCacheManager.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/GlobalCacheManager.cpp -o $(outtmp)/globalcachemanager.o

