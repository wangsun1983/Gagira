#include "TemplateReturnItem.hpp"

using namespace obotcha;

namespace gagira {

TemplateScopedValue _TemplateReturnItem::execute(TemplateScopedValueContainer scopedValueContainer,TemplateObjectContainer objContainer) {
    TemplateScopedValue value = nullptr;
    if(mFuncItem != nullptr) {
        value = mFuncItem->execute(scopedValueContainer,objContainer);
    } else {
        value = TemplateScopedValue::New(String::New(""));
    }
    value->setDirectReturn(true);
    return value;
}

void _TemplateReturnItem::setFunctionItem(TemplateItem item) {
    mFuncItem = item;
}

}
