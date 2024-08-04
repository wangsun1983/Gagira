#include "ConversationDao.hpp"

using namespace obotcha;
using namespace gagira;


ArrayList<Conversation> _ConversationDao::getConversation(String groupname) {
    SqlQuery query = SqlQuery::New("select * from $1_conversation_table");
    query->bindParam(groupname);
    return connection->query<Conversation>(query);
}

ArrayList<Conversation> _ConversationDao::getConversationAfter(String groupname,long time) {
    SqlQuery query = SqlQuery::New("select * from $1_conversation_table where time > $2");
    query->bindParam(groupname,String::New((uint64_t)time));
    return connection->query<Conversation>(query);
}

int _ConversationDao::addConversation(String groupname,
                                       long time,
                                       String username,
                                       int type,
                                       String data) {
    SqlQuery query = SqlQuery::New("insert into $1_conversation_table(time,username,type,data) values($2,'$3',$4,'$5')");
    query->bindParam(groupname,time,username,type,data);
    printf("ConversationData query is %s \n",query->toString()->toChars());
    return -connection->exec(query);
}


int _ConversationDao::createConversation(String groupname) {
    SqlQuery query = SqlQuery::New("CREATE TABLE $1_conversation_table (id INT NOT NULL AUTO_INCREMENT,\
                                    time BIGINT(64) NULL,\
                                    username VARCHAR(45) NULL,\
                                    type INT NULL,\
                                    data VARCHAR(128) NULL,\
                                    PRIMARY KEY (id))");

    query->bindParam(groupname);
    return -connection->exec(query);
}
