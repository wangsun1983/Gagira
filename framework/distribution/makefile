distributionDir = ./framework/distribution

include ./framework/distribution/broadcast/makefile
include ./framework/distribution/map/makefile
include ./framework/distribution/queue/makefile
include ./framework/distribution/archive/makefile
include ./framework/distribution/fence/makefile

objs += $(outtmp)/distributemessageparser.o \
		$(outtmp)/distributelinker.o \
		$(outtmp)/distributecenter.o \
		$(outtmp)/distributeoption.o \
		$(outtmp)/distributecenterbuilder.o \
		$(outtmp)/distributelock.o \
		$(outtmp)/distributereadwritelock.o \

$(outtmp)/distributereadwritelock.o :$(distributionDir)/DistributeReadWriteLock.cpp
	$(cppcompiler) $(gagiracppflags) -c $(distributionDir)/DistributeReadWriteLock.cpp -o $(outtmp)/distributereadwritelock.o

$(outtmp)/distributelock.o :$(distributionDir)/DistributeLock.cpp
	$(cppcompiler) $(gagiracppflags) -c $(distributionDir)/DistributeLock.cpp -o $(outtmp)/distributelock.o

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
