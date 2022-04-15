#include "HtmlTemplateReferItem.hpp"

using namespace obotcha;

namespace gagira {

ThreadLocal<HashMap<String,String>> _HtmlTemplateReferItem::values = createThreadLocal<HashMap<String,String>>();

_HtmlTemplateReferItem::_HtmlTemplateReferItem(String name) {
    paramName = name;
}

String _HtmlTemplateReferItem::getName() {
    return paramName;
}

void _HtmlTemplateReferItem::setValue(String param,String value) {
    auto map = values->get();
    if(map == nullptr) {
        map = createHashMap<String,String>();
        values->set(map);
    }

    map->put(param,value);
}

String _HtmlTemplateReferItem::toString(Object o) {
    auto map = values->get();
    if(map != nullptr) {
        return map->get(paramName);
    }

    return nullptr;
}

}
