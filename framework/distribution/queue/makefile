queueDir = ./framework/distribution/queue

objs += $(outtmp)/queuecenter.o \
		$(outtmp)/queuemessage.o \
		$(outtmp)/queueconnection.o  \
		$(outtmp)/queuemessagebuilder.o

$(outtmp)/queuecenter.o :$(queueDir)/QueueCenter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(queueDir)/QueueCenter.cpp -o $(outtmp)/queuecenter.o

$(outtmp)/queuemessage.o :$(queueDir)/QueueMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(queueDir)/QueueMessage.cpp -o $(outtmp)/queuemessage.o

$(outtmp)/queueconnection.o :$(queueDir)/QueueConnection.cpp
	$(cppcompiler) $(gagiracppflags) -c $(queueDir)/QueueConnection.cpp -o $(outtmp)/queueconnection.o

$(outtmp)/queuemessagebuilder.o :$(queueDir)/QueueMessageBuilder.cpp
	$(cppcompiler) $(gagiracppflags) -c $(queueDir)/QueueMessageBuilder.cpp -o $(outtmp)/queuemessagebuilder.o
