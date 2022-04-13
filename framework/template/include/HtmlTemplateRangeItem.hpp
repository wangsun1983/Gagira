#ifndef __GAGRIA_HTML_TEMPLATE_RANGE_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_RANGE_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HtmlTemplateRangeItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    _HtmlTemplateRangeItem(String);
    String toString(Object o);

private:
    String text;
    ArrayList<HtmlTemplateItem> items;

};

}

#endif