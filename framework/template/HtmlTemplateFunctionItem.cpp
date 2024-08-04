#include "HtmlTemplateFunctionItem.hpp"

using namespace obotcha;

namespace gagira {

_HtmlTemplateFunctionItem::_HtmlTemplateFunctionItem(String name,HtmlTemplateFunction func) {
    mName = name;
    mItems = ArrayList<HtmlTemplateItem>::New();
}

String _HtmlTemplateFunctionItem::toString(Object o) {
    ArrayList<String> params = ArrayList<String>::New();
    auto iterator = mItems->getIterator();
    while(iterator->hasValue()) {
        params->add(iterator->getValue()->toString(o));
        iterator->next();
    }

    return mFunc(params);
}



}
