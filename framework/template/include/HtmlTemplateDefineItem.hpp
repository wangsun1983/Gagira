#ifndef __GAGRIA_HTML_TEMPLATE_DEFINE_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_DEFINE_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "HtmlTemplate.hpp"
#include "ArrayList.hpp"
#include "HtmlTemplateReferItem.hpp"

using namespace obotcha;

class _TemplateDefineCmdParser;
class _HtmlTemplate;

namespace gagira {

DECLARE_CLASS(HtmlTemplateDefineItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    friend class _TemplateDefineCmdParser;
    friend class _HtmlTemplate;
    _HtmlTemplateDefineItem();
    String toString(Object o);

private:
    String mName;
    HtmlTemplate mTemplate;
};

}

#endif
