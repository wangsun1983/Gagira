#ifndef __GAGRIA_HTML_TEMPLATE_REFER_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_REFER_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "HtmlTemplateObjectItem.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HtmlTemplateReferItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    _HtmlTemplateReferItem();
    String toString(Object o);

private:
    HtmlTemplateObjectItem objItem;
    String stringItem;
};

}

#endif
