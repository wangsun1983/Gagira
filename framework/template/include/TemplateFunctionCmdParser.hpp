#ifndef __GAGRIA_HTML_TEMPLATE_FUNCTION_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_FUNCTION_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "TemplateCmdParser.hpp"
#include "HtmlTemplateFunctionItem.hpp"
#include "HtmlTemplateFunction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateFunctionCmdParser) IMPLEMENTS(TemplateCmdParser){
public:
    _TemplateFunctionCmdParser(String name,HtmlTemplateFunction func);
    void doParse(String);
    HtmlTemplateItem getTemplateItem();

private:
    String mName;
    HtmlTemplateFunctionItem mItem;
    HtmlTemplateFunction mFunc;
};

}

#endif
