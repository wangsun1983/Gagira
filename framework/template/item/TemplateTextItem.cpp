#include "TemplateTextItem.hpp"

namespace gagira {

_TemplateTextItem::_TemplateTextItem(String text) {
    mType = st(TemplateItem)::Type::Text;
    mText = text;
}

TemplateScopedValue _TemplateTextItem::execute(TemplateScopedValueContainer,TemplateObjectContainer)  {
    return TemplateScopedValue::New(mText);
}

}

