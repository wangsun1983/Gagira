#include "TemplateBreakItem.hpp"

using namespace obotcha;

namespace gagira {

TemplateScopedValue _TemplateBreakItem::execute(TemplateScopedValueContainer,TemplateObjectContainer) {
    auto value = TemplateScopedValue::New(String::New(""));
    value->setDirectBreak(true);
    return value;
}

}
