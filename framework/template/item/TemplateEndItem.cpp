#include "TemplateEndItem.hpp"

using namespace obotcha;

namespace gagira {

_TemplateEndItem::_TemplateEndItem() {
    //TODO
}

TemplateScopedValue _TemplateEndItem::execute(TemplateScopedValueContainer,TemplateObjectContainer objContainer) {
    objContainer->removeCurrent();
    return nullptr;
}

}

