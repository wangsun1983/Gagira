#ifndef __GAGRIA_MY_SQL_DAO_HPP__
#define __GAGRIA_MY_SQL_DAO_HPP__

#include "String.hpp"
#include "Dao.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(MySqlDao) IMPLEMENTS(Dao){
public:
    _MySqlDao();
};

} // namespace gagira

#endif