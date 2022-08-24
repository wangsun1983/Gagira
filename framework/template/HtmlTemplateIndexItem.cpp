#include "HtmlTemplateIndexItem.hpp"

using namespace obotcha;

namespace gagira {


_HtmlTemplateIndexItem::_HtmlTemplateIndexItem() {
    index = 0;
}

String _HtmlTemplateIndexItem::toString(Object o) {
    //TODO
    return nullptr;
}

Object _HtmlTemplateIndexItem::toObject(Object o) {
    if(index >= o->__getContainerSize("")) {
        return nullptr;
    }

    return o->__getListItemObject("",index);
}



}