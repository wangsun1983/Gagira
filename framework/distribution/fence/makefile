fenceDir = ./framework/distribution/fence

objs += $(outtmp)/fencemessage.o \
		$(outtmp)/fencecenter.o \
		$(outtmp)/fence.o \
		$(outtmp)/fenceconnection.o \

$(outtmp)/fencemessage.o :$(fenceDir)/FenceMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(fenceDir)/FenceMessage.cpp -o $(outtmp)/fencemessage.o

$(outtmp)/fencecenter.o :$(fenceDir)/FenceCenter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(fenceDir)/FenceCenter.cpp -o $(outtmp)/fencecenter.o

$(outtmp)/fence.o :$(fenceDir)/Fence.cpp
	$(cppcompiler) $(gagiracppflags) -c $(fenceDir)/Fence.cpp -o $(outtmp)/fence.o

$(outtmp)/fenceconnection.o :$(fenceDir)/FenceConnection.cpp
	$(cppcompiler) $(gagiracppflags) -c $(fenceDir)/FenceConnection.cpp -o $(outtmp)/fenceconnection.o
