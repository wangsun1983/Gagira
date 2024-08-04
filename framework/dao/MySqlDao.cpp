#include "MySqlDao.hpp"
#include "MySqlConnectParam.hpp"
#include "MySqlConnection.hpp"
#include "Configs.hpp"

using namespace obotcha;

namespace gagira {

_MySqlDao::_MySqlDao() {
    connection = MySqlConnection::New();
    MySqlConnectParam param = MySqlConnectParam::New();

    ArrayList<SqlConfig> confs = st(Configs)::getInstance()->getSqlConfigs();
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
