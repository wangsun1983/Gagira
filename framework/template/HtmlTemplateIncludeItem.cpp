#include "HtmlTemplateIncludeItem.hpp"

using namespace obotcha;

namespace gagira {

_HtmlTemplateIncludeItem::_HtmlTemplateIncludeItem() {
    templateItem = nullptr;
}

String _HtmlTemplateIncludeItem::toString(Object o) {
    if(templateItem != nullptr) {
        return templateItem->execute(o);
    }

    return nullptr;
}


}
