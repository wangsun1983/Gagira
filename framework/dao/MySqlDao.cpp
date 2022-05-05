#include "MySqlDao.hpp"
#include "MySqlConnectParam.hpp"
#include "MySqlConnection.hpp"
#include "Configs.hpp"

using namespace obotcha;

namespace gagira {

_MySqlDao::_MySqlDao() {
    connection = createMySqlConnection();
    MySqlConnectParam param = createMySqlConnectParam();

    ArrayList<SqlConfig> confs = st(Configs)::getInstance()->items->sqlconfigs;
    auto iterator = confs->getIterator();
    while(iterator->hasValue()) {
        SqlConfig config = iterator->getValue();
        if(config->type->equalsIgnoreCase("MySql")) {
            param->setHost(config->ip);
            param->setUser(config->username);
            param->setPassword(config->password);
            param->setDbName(config->dbname);
            param->setPort(config->port);
            connection->connect(param);
            break;
        }
        iterator->next();
    }
}

} // namespace gagira
