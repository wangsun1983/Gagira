#ifndef __MY_GROUP_GROUP_INFO_HPP__
#define __MY_GROUP_GROUP_INFO_HPP__

#include "String.hpp"
#include "Reflect.hpp"

using namespace obotcha;

DECLARE_CLASS(GroupInfo) {
public:
    String group_id;
    String group_name;
    String group_type;
    String group_summary;
    String group_owner;
    
    DECLARE_REFLECT_FIELD(GroupInfo,group_id,group_name,group_summary,group_type,group_owner)
};


#endif