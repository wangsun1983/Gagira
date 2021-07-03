frameworkDir = framework

objs += $(outtmp)/controller.o \
		$(outtmp)/server.o \
		$(outtmp)/servletrequest.o \
		$(outtmp)/servletrequestmgr.o \
		

$(outtmp)/controller.o :$(frameworkDir)/Controller.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/Controller.cpp -o $(outtmp)/controller.o

$(outtmp)/server.o :$(frameworkDir)/Server.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/Server.cpp -o $(outtmp)/server.o

$(outtmp)/servletrequest.o :$(frameworkDir)/ServletRequest.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/ServletRequest.cpp -o $(outtmp)/servletrequest.o

$(outtmp)/servletrequestmgr.o :$(frameworkDir)/ServletRequestManager.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDir)/ServletRequestManager.cpp -o $(outtmp)/servletrequestmgr.o
