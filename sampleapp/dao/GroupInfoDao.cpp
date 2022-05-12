#include "GroupInfoDao.hpp"
#include "StringBuffer.hpp"
#include "Log.hpp"

ArrayList<GroupInfo> _GroupInfoDao::getOwnGroups(String username) {
    SqlQuery query = createSqlQuery("select * from group_table where group_owner = '$1'");
    query->bindParam(username);
    printf("getOwnGroups query is %s \n",query->toString()->toChars());
    return connection->query<GroupInfo>(query);
}

ArrayList<GroupInfo> _GroupInfoDao::getFollowedGroups(String username) {
    SqlQuery query = createSqlQuery("SELECT * FROM group_follower_table r Join group_table using(group_name,group_id) where user_name = '$1'");
    query->bindParam(username);
    return connection->query<GroupInfo>(query);
}

int _GroupInfoDao::addNewGroup(String group_name,String group_type,String group_owner,String group_summary) {
    SqlQuery query = createSqlQuery("insert into group_table(group_name,group_type,group_owner,group_summary) VALUES('$1','$2','$3','$4')");
    query->bindParam(group_name,group_type,group_owner,group_summary);

    return connection->exec(query);
}

int _GroupInfoDao::followGroup(String group_name,String username) {
    SqlQuery query = createSqlQuery("insert into group_follower_table(user_name,group_name) VALUES('$1','$2')");
    query->bindParam(group_name,username);
    return connection->exec(query);
}

GroupInfo _GroupInfoDao::getGroupInfo(String groupname) {
    SqlQuery query = createSqlQuery("SELECT * FROM group_table where groupname = '$1'");
    query->bindParam(groupname);
    auto list = connection->query<GroupInfo>(query);
    if(list->size() > 1) {
        LOG(ERROR)<<"Get group size > 1";
    }

    return list->get(0);
}