controllerDir = controller

objs += $(outtmp)/contorllermanager.o \
		$(outtmp)/controller.o 

$(outtmp)/contorllermanager.o :$(controllerDir)/ControllerManager.cpp
	$(cppcompiler) $(gagiracppflags) -c $(controllerDir)/ControllerManager.cpp -o $(outtmp)/contorllermanager.o

$(outtmp)/controller.o :$(controllerDir)/Controller.cpp
	$(cppcompiler) $(gagiracppflags) -c $(controllerDir)/Controller.cpp -o $(outtmp)/controller.o
