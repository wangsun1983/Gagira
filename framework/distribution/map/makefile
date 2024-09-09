spaceDir = ./framework/distribution/map

objs += $(outtmp)/mapcenter.o \
		$(outtmp)/mapmessage.o \
		$(outtmp)/mapconnection.o 

$(outtmp)/mapcenter.o :$(spaceDir)/MapCenter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(spaceDir)/MapCenter.cpp -o $(outtmp)/mapcenter.o

$(outtmp)/mapmessage.o :$(spaceDir)/MapMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(spaceDir)/MapMessage.cpp -o $(outtmp)/mapmessage.o

$(outtmp)/mapconnection.o :$(spaceDir)/MapConnection.cpp
	$(cppcompiler) $(gagiracppflags) -c $(spaceDir)/MapConnection.cpp -o $(outtmp)/mapconnection.o
