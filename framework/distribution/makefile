distributionDir = ./framework/distribution

include ./framework/distribution/broadcast/makefile
include ./framework/distribution/space/makefile
include ./framework/distribution/queue/makefile
include ./framework/distribution/archive/makefile

objs += $(outtmp)/distributemessageparser.o \
		$(outtmp)/distributelinker.o \
		$(outtmp)/distributecenter.o \
		$(outtmp)/distributeoption.o \
		$(outtmp)/distributecenterbuilder.o

$(outtmp)/distributemessageparser.o :$(distributionDir)/DistributeMessageParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(distributionDir)/DistributeMessageParser.cpp -o $(outtmp)/distributemessageparser.o

$(outtmp)/distributelinker.o :$(distributionDir)/DistributeLinker.cpp
	$(cppcompiler) $(gagiracppflags) -c $(distributionDir)/DistributeLinker.cpp -o $(outtmp)/distributelinker.o

$(outtmp)/distributecenter.o :$(distributionDir)/DistributeCenter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(distributionDir)/DistributeCenter.cpp -o $(outtmp)/distributecenter.o

$(outtmp)/distributeoption.o :$(distributionDir)/DistributeOption.cpp
	$(cppcompiler) $(gagiracppflags) -c $(distributionDir)/DistributeOption.cpp -o $(outtmp)/distributeoption.o

$(outtmp)/distributecenterbuilder.o :$(distributionDir)/DistributeCenterBuilder.cpp
	$(cppcompiler) $(gagiracppflags) -c $(distributionDir)/DistributeCenterBuilder.cpp -o $(outtmp)/distributecenterbuilder.o
