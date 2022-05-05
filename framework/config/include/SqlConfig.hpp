#ifndef __GAGRIA_SERVER_SQL_CONFIG_HPP__
#define __GAGRIA_SERVER_SQL_CONFIG_HPP__

#include "String.hpp"
#include "Reflect.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(SqlConfig) {
public:
    String type; //MySql/Sqlite...
    String ip;
    int port;
    String username;
    String password;
    String dbname;

    DECLARE_REFLECT_FIELD(SqlConfig,type,ip,port,username,password,dbname)
};

} // namespace gagira

#endif