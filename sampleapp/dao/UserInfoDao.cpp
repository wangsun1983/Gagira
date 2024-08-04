#include "UserInfoDao.hpp"
#include "StringBuffer.hpp"

UserInfo _UserInfoDao::getUserInfo(String username,String password) {
    SqlQuery query = nullptr;
    if(password != nullptr) {
        query = SqlQuery::New("select * from user_table where username ='$1' AND password='$2'");
        query->bindParam(username,password);
    } else {
        query = SqlQuery::New("select * from user_table where username ='$1'");
        query->bindParam(username);
    }

    ArrayList<UserInfo> result = connection->query<UserInfo>(query);
    if(result != nullptr) {
        return result->get(0);
    }

    return nullptr;
}

int _UserInfoDao::addNewUser(String username,String password) {
    SqlQuery query = SqlQuery::New("insert into user_table(username,password) VALUES('$1','$2')");
    query->bindParam(username,password);
    return -connection->exec(query);
}

