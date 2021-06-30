controllerDir = controller

objs += $(outtmp)/controller.o \
		$(outtmp)/server.o 

$(outtmp)/controller.o :$(controllerDir)/Controller.cpp
	$(cppcompiler) $(gagiracppflags) -c $(controllerDir)/Controller.cpp -o $(outtmp)/controller.o

$(outtmp)/server.o :$(controllerDir)/Server.cpp
	$(cppcompiler) $(gagiracppflags) -c $(controllerDir)/Server.cpp -o $(outtmp)/server.o
