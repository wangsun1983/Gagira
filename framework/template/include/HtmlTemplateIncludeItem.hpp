#ifndef __GAGRIA_HTML_TEMPLATE_INCLUDE_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_INCLUDE_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "HtmlTemplate.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

namespace gagira {

class _HtmlTemplate;
class _TemplateIncludeCmdParser;

DECLARE_CLASS(HtmlTemplateIncludeItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    friend class _TemplateIncludeCmdParser;
    friend class _HtmlTemplate;

    _HtmlTemplateIncludeItem();
    String toString(Object o);

private:
    HtmlTemplate templateItem;
};

}

#endif
