mqDir = ./framework/mq

objs += $(outtmp)/mqcenter.o \
		$(outtmp)/mqoption.o \
		$(outtmp)/mqmessage.o \
		$(outtmp)/mqconnection.o \
		$(outtmp)/mqcenterbuilder.o \
		$(outtmp)/mqdefaultpersistence.o 


$(outtmp)/mqcenter.o :$(frameworkDir)/mq/MqCenter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqCenter.cpp -o $(outtmp)/mqcenter.o

$(outtmp)/mqoption.o :$(frameworkDir)/mq/MqOption.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqOption.cpp -o $(outtmp)/mqoption.o

$(outtmp)/mqcenterbuilder.o :$(frameworkDir)/mq/MqCenterBuilder.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqCenterBuilder.cpp -o $(outtmp)/mqcenterbuilder.o

$(outtmp)/mqdefaultpersistence.o :$(frameworkDir)/mq/MqDefaultPersistence.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqDefaultPersistence.cpp -o $(outtmp)/mqdefaultpersistence.o

$(outtmp)/mqmessage.o :$(frameworkDir)/mq/MqMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqMessage.cpp -o $(outtmp)/mqmessage.o

$(outtmp)/mqconnection.o :$(frameworkDir)/mq/MqConnection.cpp
	$(cppcompiler) $(gagiracppflags) -c $(mqDir)/MqConnection.cpp -o $(outtmp)/mqconnection.o
