#ifndef __GAGRIA_HTML_TEMPLATE_WITH_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_WITH_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "HtmlTemplateObjectItem.hpp"
#include "ArrayList.hpp"
#include "ThreadLocal.hpp"
#include "HashMap.hpp"
#include "HtmlTemplate.hpp"

using namespace obotcha;

namespace gagira {

class _TemplateWithCmdParser;
class _HtmlTemplate;

DECLARE_CLASS(HtmlTemplateWithItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    friend class _TemplateWithCmdParser;
    friend class _HtmlTemplate;
    _HtmlTemplateWithItem();
    String toString(Object o);

private:
    HtmlTemplateItem pipline;
    HtmlTemplate content;
};

}

#endif
