#ifndef __GAGRIA_HTML_TEMPLATE_FUNCTION_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_FUNCTION_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "HtmlTemplateFunction.hpp"
#include "HtmlTemplate.hpp"

using namespace obotcha;

namespace gagira {

class _TemplateFunctionCmdParser;
class _HtmlTemplate;

DECLARE_CLASS(HtmlTemplateFunctionItem) IMPLEMENTS(HtmlTemplateItem) {

public:
    friend class _TemplateFunctionCmdParser;
    friend class _HtmlTemplate;
    _HtmlTemplateFunctionItem(String name,HtmlTemplateFunction);
    String toString(Object o);

private:
    HtmlTemplateFunction mFunc;
    ArrayList<HtmlTemplateItem> mItems;
    String mName;
    
};

}

#endif
