#include "HtmlTemplateDefineItem.hpp"

namespace gagira {

_HtmlTemplateDefineItem::_HtmlTemplateDefineItem() {

}

String _HtmlTemplateDefineItem::toString(Object o) {
    if(mTemplate != nullptr) {
        return mTemplate->execute(o);
    }

    return nullptr;
}



}
