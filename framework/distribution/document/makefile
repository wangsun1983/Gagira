docDir = ./framework/distribution/document

objs += $(outtmp)/documentlinker.o \
		$(outtmp)/documentcenter.o \
		$(outtmp)/documentmessage.o \
		$(outtmp)/documentoption.o \
		$(outtmp)/documentconnection.o 

$(outtmp)/documentlinker.o :$(docDir)/DocumentLinker.cpp
	$(cppcompiler) $(gagiracppflags) -c $(docDir)/DocumentLinker.cpp -o $(outtmp)/documentlinker.o

$(outtmp)/documentcenter.o :$(docDir)/DocumentCenter.cpp
	$(cppcompiler) $(gagiracppflags) -c $(docDir)/DocumentCenter.cpp -o $(outtmp)/documentcenter.o

$(outtmp)/documentmessage.o :$(docDir)/DocumentMessage.cpp
	$(cppcompiler) $(gagiracppflags) -c $(docDir)/DocumentMessage.cpp -o $(outtmp)/documentmessage.o

$(outtmp)/documentoption.o :$(docDir)/DocumentOption.cpp
	$(cppcompiler) $(gagiracppflags) -c $(docDir)/DocumentOption.cpp -o $(outtmp)/documentoption.o

$(outtmp)/documentconnection.o :$(docDir)/DocumentConnection.cpp
	$(cppcompiler) $(gagiracppflags) -c $(docDir)/DocumentConnection.cpp -o $(outtmp)/documentconnection.o
