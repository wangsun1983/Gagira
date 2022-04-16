#ifndef __GAGRIA_HTML_TEMPLATE_LEN_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_LEN_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

class _TemplateLenCmdParser;

namespace gagira {

DECLARE_CLASS(HtmlTemplateLenItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    friend class _TemplateLenCmdParser;

    _HtmlTemplateLenItem();
    String toString(Object o);

};

}

#endif
