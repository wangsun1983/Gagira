#ifndef __GAGRIA_DAO_HPP__
#define __GAGRIA_DAO_HPP__

#include "String.hpp"
#include "SqlConnection.hpp"

using namespace obotcha;

namespace gagira  {

DECLARE_CLASS(Dao) {
public:
    SqlConnection getConnection();

protected:
    SqlConnection connection;
};

}

#endif