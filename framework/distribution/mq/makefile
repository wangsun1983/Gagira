mqDir = ./framework/distribution/mq

objs += $(outtmp)/mqcenter.o \
		$(outtmp)/mqmessage.o \
		$(outtmp)/mqconnection.o \
		$(outtmp)/mqsustainmessage.o \
		$(outtmp)/mqdlqmessage.o 

$(outtmp)/mqcenter.o :$(mqDir)/MqCenter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqCenter.cpp -o $(outtmp)/mqcenter.o

$(outtmp)/mqmessage.o :$(mqDir)/MqMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqMessage.cpp -o $(outtmp)/mqmessage.o

$(outtmp)/mqconnection.o :$(mqDir)/MqConnection.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqConnection.cpp -o $(outtmp)/mqconnection.o

$(outtmp)/mqsustainmessage.o :$(mqDir)/MqSustainMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqSustainMessage.cpp -o $(outtmp)/mqsustainmessage.o

$(outtmp)/mqdlqmessage.o :$(mqDir)/MqDLQMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqDLQMessage.cpp -o $(outtmp)/mqdlqmessage.o
