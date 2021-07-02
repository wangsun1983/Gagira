outlib = ./out/lib
outtmp = ./out/tmp

#gcc = g++
ccompiler = clang
cppcompiler = clang++
clangcompiler = clang++

gcc = g++

cflags = -fpic \
		-lrt \
		-I ./3rdparty/Obotcha/include/util/concurrent/ \
		-I ./3rdparty/Obotcha/include/util/coroutine/ \
		-I ./3rdparty/Obotcha/include/util/text/ \
		-I ./3rdparty/Obotcha/include/util/sql/ \
		-I ./3rdparty/Obotcha/include/lang/ \
		-I ./3rdparty/Obotcha/include/io/ \
		-I ./3rdparty/Obotcha/include/util/ \
		-I ./3rdparty/Obotcha/include/net/ \
		-I ./3rdparty/Obotcha/include/security/ \
		-I ./3rdparty/Obotcha/include/process/ \
		-I ./3rdparty/Obotcha/include/external/iniparser/ \
		-I ./3rdparty/Obotcha/include/external/jsoncpp/ \
		-I ./3rdparty/Obotcha/include/external/rapidxml/ \
		-I ./3rdparty/Obotcha/include/external/zlib/ \
		-I ./3rdparty/Obotcha/include/external/ccl/ \
		-I ./3rdparty/Obotcha/include/external/yaml/ \
		-I ./3rdparty/Obotcha/include/external/crc32/ \
		-I ./3rdparty/Obotcha/include/external/openssl/ \
		-I ./3rdparty/Obotcha/include/external/ \
		-I ./3rdparty/Obotcha/include/external/sqlite3/ \
		-I ./3rdparty/Obotcha/include/external/http_parser/ \
		-I ./3rdparty/Obotcha/include/external/uuid/ \
		-I ./3rdparty/Obotcha/include/external/glog/ \
		-I ./include/ \
		-g \
		-std=c++14 \

external = -lpthread \
				-ldl \
				-L ./3rdparty/Obotcha/libobotcha.so \

sharelib = -Wl,-rpath=./3rdparty/Obotcha/ \
           -L ./3rdparty/Obotcha/ \

objs =
link =
libname	= gagira

cppflags=$(cflags) \
		 -std=c++14

gagiracppflags = $(cppflags)

everything : $(libname)

include controller/makefile

$(libname): $(objs)
	g++ -g -o0 main.cpp $(cflags) $(sharelib) -o $(outlib)/gagira $(staticlib) $(external)

