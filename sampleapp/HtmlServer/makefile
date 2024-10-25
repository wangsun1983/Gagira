outlib = ../../out/lib
outtmp = ../../out/tmp

#gcc = g++
ccompiler = clang
cppcompiler = clang++
clangcompiler = clang++

gcc = g++

cflags = -fpic \
		-I ../../3rdparty/Obotcha/include/util/concurrent/ \
		-I ../../3rdparty/Obotcha/include/util/coroutine/ \
		-I ../../3rdparty/Obotcha/include/util/text/ \
		-I ../../3rdparty/Obotcha/include/util/sql/ \
        -I ../../3rdparty/Obotcha/include/util/time/ \
		-I ../../3rdparty/Obotcha/include/lang/ \
		-I ../../3rdparty/Obotcha/include/io/ \
		-I ../../3rdparty/Obotcha/include/util/ \
		-I ../../3rdparty/Obotcha/include/net/ \
        -I ../../3rdparty/Obotcha/include/net/socket/ \
        -I ../../3rdparty/Obotcha/include/sql/ \
        -I ../../3rdparty/Obotcha/include/net/websocket/ \
        -I ../../3rdparty/Obotcha/include/net/http/ \
		-I ../../3rdparty/Obotcha/include/security/ \
		-I ../../3rdparty/Obotcha/include/process/ \
		-I ../../3rdparty/Obotcha/include/external/iniparser/ \
		-I ../../3rdparty/Obotcha/include/external/jsoncpp/ \
		-I ../../3rdparty/Obotcha/include/external/rapidxml/ \
		-I ../../3rdparty/Obotcha/include/external/zlib/ \
		-I ../../3rdparty/Obotcha/include/external/ccl/ \
		-I ../../3rdparty/Obotcha/include/external/yaml/ \
		-I ../../3rdparty/Obotcha/include/external/crc32/ \
		-I ../../3rdparty/Obotcha/include/external/openssl/ \
		-I ../../3rdparty/Obotcha/include/external/ \
		-I ../../3rdparty/Obotcha/include/external/sqlite3/ \
		-I ../../3rdparty/Obotcha/include/external/http_parser/ \
		-I ../../3rdparty/Obotcha/include/external/uuid/ \
		-I ../../3rdparty/Obotcha/include/external/glog/ \
		-I ../../framework/include/ \
        -I ../../framework/config/include/ \
		-I ../../framework/template/include/ \
		-g \
		-std=c++17 \
		-fsanitize=address \
		-fno-omit-frame-pointer

external = -lpthread \
				-ldl \
				-L ../../3rdparty/Obotcha/libobotcha.so \
				-L ../../out/lib/libgagira.so

sharelib = -Wl,-rpath=../../3rdparty/Obotcha/:../../out/lib/ \
           -L ../../3rdparty/Obotcha/ \
		   -lobotcha \
		   -L ../../out/lib/ \
		   -lgagira \

objs =
link =
libname	= gagira

cppflags=$(cflags) \
		 -std=c++17

gagiracppflags = $(cppflags)

everything : $(libname)

$(libname): $(objs)
	$(cppcompiler) *.cpp $(cflags) $(objs) -o mytest $(external) $(sharelib)

