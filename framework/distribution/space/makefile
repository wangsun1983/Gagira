spaceDir = ./framework/distribution/space

objs += $(outtmp)/spacecenter.o \
		$(outtmp)/spacemessage.o

$(outtmp)/spacecenter.o :$(spaceDir)/SpaceCenter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(spaceDir)/SpaceCenter.cpp -o $(outtmp)/spacecenter.o

$(outtmp)/spacemessage.o :$(spaceDir)/SpaceMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(spaceDir)/SpaceMessage.cpp -o $(outtmp)/spacemessage.o
