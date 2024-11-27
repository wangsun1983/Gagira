#ifndef __GAGRIA_TEMPLATE_IN_ITEM_HPP__
#define __GAGRIA_TEMPLATE_IN_ITEM_HPP__

#include "TemplateItem.hpp"
#include "TemplateScopedValue.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateInItem) IMPLEMENTS(TemplateItem){
public:
    TemplateScopedValue execute(TemplateScopedValueContainer,TemplateObjectContainer);
    void setInName(String);
    
private:
    String mFieldName;
};

}

#endif
