#include "TemplateInItem.hpp"
#include "IllegalArgumentException.hpp"
#include "Field.hpp"

using namespace obotcha;

namespace gagira {

TemplateScopedValue _TemplateInItem::execute(TemplateScopedValueContainer ,TemplateObjectContainer container) {
    auto current_obj = container->getCurrent();
    auto field = current_obj->getField(mFieldName);
    if(field != nullptr) {
        auto obj = field->getObjectValue();;
        if(obj != nullptr) {
            container->addNew(obj);
            return nullptr;
        }
    }

    Trigger(IllegalArgumentException)
}

void _TemplateInItem::setInName(String name) {
    mFieldName = name;
}

}


