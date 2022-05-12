#ifndef __GAGIRA_NONE_RESPONSE_ENTITY_HPP__
#define __GAGIRA_NONE_RESPONSE_ENTITY_HPP__

#include "Object.hpp"
#include "StrongPointer.hpp"

#include "HttpResponseEntity.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(NoneResponseEntity) IMPLEMENTS(HttpResponseEntity){

public:
    _NoneResponseEntity():_HttpResponseEntity(NoResponse) {}
};

}
#endif
