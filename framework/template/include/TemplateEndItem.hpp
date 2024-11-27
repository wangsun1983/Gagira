#ifndef __GAGRIA_TEMPLATE_END_ITEM_HPP__
#define __GAGRIA_TEMPLATE_END_ITEM_HPP__

#include "TemplateItem.hpp"
#include "TemplateScopedValue.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateEndItem) IMPLEMENTS(TemplateItem){
public:
    TemplateScopedValue execute(TemplateScopedValueContainer,TemplateObjectContainer);
};

}

#endif