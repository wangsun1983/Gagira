frameworkDaoDir = ./framework/dao

objs += $(outtmp)/mysqldao.o  \
		$(outtmp)/dao.o 

$(outtmp)/mysqldao.o :$(frameworkDaoDir)/MySqlDao.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDaoDir)/MySqlDao.cpp -o $(outtmp)/mysqldao.o

$(outtmp)/dao.o :$(frameworkDaoDir)/Dao.cpp
	$(cppcompiler) $(gagiracppflags) -c $(frameworkDaoDir)/Dao.cpp -o $(outtmp)/dao.o
