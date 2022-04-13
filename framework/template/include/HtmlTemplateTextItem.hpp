#ifndef __GAGRIA_HTML_TEMPLATE_TEXT_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_TEXT_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HtmlTemplateTextItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    _HtmlTemplateTextItem(String);
    String toString(Object o);

private:
    String text;
    
};

}

#endif
