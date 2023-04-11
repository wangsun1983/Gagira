mqDir = ./framework/distribution/broadcast

objs += $(outtmp)/broadcastcenter.o \
		$(outtmp)/broadcastmessage.o \
		$(outtmp)/broadcastconnection.o \
		$(outtmp)/broadcastsustainmessage.o \
		$(outtmp)/broadcastdlqmessage.o 

$(outtmp)/broadcastcenter.o :$(mqDir)/BroadcastCenter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/BroadcastCenter.cpp -o $(outtmp)/broadcastcenter.o

$(outtmp)/broadcastmessage.o :$(mqDir)/BroadcastMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/BroadcastMessage.cpp -o $(outtmp)/broadcastmessage.o

$(outtmp)/broadcastconnection.o :$(mqDir)/BroadcastConnection.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/BroadcastConnection.cpp -o $(outtmp)/broadcastconnection.o

$(outtmp)/broadcastsustainmessage.o :$(mqDir)/BroadcastSustainMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/BroadcastSustainMessage.cpp -o $(outtmp)/broadcastsustainmessage.o

$(outtmp)/broadcastdlqmessage.o :$(mqDir)/BroadcastDLQMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/BroadcastDLQMessage.cpp -o $(outtmp)/broadcastdlqmessage.o
