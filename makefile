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
		-I ./framework/include/ \
		-g \
		-std=c++14 \

external = -lpthread \
				-ldl \
				-L ./3rdparty/Obotcha/libobotcha.so \

#-L./traderapi 是使用到的动态链接库的路径
#-lthostmduserapi 是动态链接库，注意动态链接库本身必须是libXXXX.so这种写法，编译的时候省略"lib"和".so"

sharelib = -Wl,-rpath=./3rdparty/Obotcha/ \
           -L ./3rdparty/Obotcha/ \
		   -lobotcha \

objs =
link =
libname	= gagira

cppflags=$(cflags) \
		 -std=c++14

gagiracppflags = $(cppflags)

everything : $(libname)

include framework/makefile

$(libname): $(objs)
	$(cppcompiler) *.cpp $(cflags) $(objs) -o gagria_server $(external) $(sharelib)

