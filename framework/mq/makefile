mqDir = ./framework/mq

objs += $(outtmp)/mqcenter.o \
		$(outtmp)/mqoption.o \
		$(outtmp)/mqmessage.o \
		$(outtmp)/mqconnection.o \
		$(outtmp)/mqcenterbuilder.o \
		$(outtmp)/mqparser.o \
		$(outtmp)/mqlinker.o \
		$(outtmp)/mqsustainmessage.o \
		$(outtmp)/mqdlqmessage.o 

$(outtmp)/mqcenter.o :$(frameworkDir)/mq/MqCenter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqCenter.cpp -o $(outtmp)/mqcenter.o

$(outtmp)/mqparser.o :$(frameworkDir)/mq/MqParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqParser.cpp -o $(outtmp)/mqparser.o

$(outtmp)/mqlinker.o :$(frameworkDir)/mq/MqLinker.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqLinker.cpp -o $(outtmp)/mqlinker.o

$(outtmp)/mqoption.o :$(frameworkDir)/mq/MqOption.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqOption.cpp -o $(outtmp)/mqoption.o

$(outtmp)/mqcenterbuilder.o :$(frameworkDir)/mq/MqCenterBuilder.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqCenterBuilder.cpp -o $(outtmp)/mqcenterbuilder.o

$(outtmp)/mqmessage.o :$(frameworkDir)/mq/MqMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqMessage.cpp -o $(outtmp)/mqmessage.o

$(outtmp)/mqconnection.o :$(frameworkDir)/mq/MqConnection.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqConnection.cpp -o $(outtmp)/mqconnection.o

$(outtmp)/mqsustainmessage.o :$(frameworkDir)/mq/MqSustainMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqSustainMessage.cpp -o $(outtmp)/mqsustainmessage.o

$(outtmp)/mqdlqmessage.o :$(frameworkDir)/mq/MqDLQMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqDLQMessage.cpp -o $(outtmp)/mqdlqmessage.o
