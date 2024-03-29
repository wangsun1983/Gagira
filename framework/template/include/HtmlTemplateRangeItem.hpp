#ifndef __GAGRIA_HTML_TEMPLATE_RANGE_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_RANGE_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "HtmlTemplate.hpp"
#include "ArrayList.hpp"
#include "HtmlTemplateReferItem.hpp"

using namespace obotcha;

class _TemplateRangeCmdParser;
class _HtmlTemplate;

namespace gagira {

DECLARE_CLASS(HtmlTemplateRangeItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    friend class _TemplateRangeCmdParser;
    friend class _HtmlTemplate;
    _HtmlTemplateRangeItem();
    String toString(Object o);

private:
    String mIndexName;
    HtmlTemplate mTemplate;
    HtmlTemplate mNoValueTemplate;

};

}

#endif
