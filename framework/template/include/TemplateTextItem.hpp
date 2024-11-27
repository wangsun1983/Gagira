#ifndef __GAGRIA_TEMPLATE_TEXT_ITEM_HPP__
#define __GAGRIA_TEMPLATE_TEXT_ITEM_HPP__

#include "String.hpp"
#include "TemplateItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateTextItem) IMPLEMENTS(TemplateItem){
public:
    _TemplateTextItem(String text);
    TemplateScopedValue execute(TemplateScopedValueContainer,TemplateObjectContainer objContainer);
    
private:
    String mText;
};

}

#endif
