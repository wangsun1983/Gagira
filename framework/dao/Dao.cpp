#include "Dao.hpp"
#include "Configs.hpp"

using namespace obotcha;

namespace gagira {

SqlConnection _Dao::getConnection() {
    return connection;
}

};
