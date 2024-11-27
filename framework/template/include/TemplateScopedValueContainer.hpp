#ifndef __GAGRIA_TEMPLATE_SCOPED_VALUE_CONTAINER_HPP__
#define __GAGRIA_TEMPLATE_SCOPED_VALUE_CONTAINER_HPP__

#include "LinkedList.hpp"
#include "TemplateScopedValue.hpp"
#include "String.hpp"
#include "HashMap.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateScopedValueContainer) {
public:
   _TemplateScopedValueContainer(HashMap<String,TemplateScopedValue>);

   HashMap<String,TemplateScopedValue> getCurrent();
   void createNew();
   void removeCurrent();

   void addScopedValue(String,TemplateScopedValue);
   TemplateScopedValue getScopedValue(String);
private:
    LinkedList<HashMap<String,TemplateScopedValue>> mScopedValues;
};

}

#endif
