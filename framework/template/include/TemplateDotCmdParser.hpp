#ifndef __GAGRIA_HTML_TEMPLATE_DOT_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_DOT_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "TemplateCmdParser.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateDotCmdParser) IMPLEMENTS(TemplateCmdParser){
public:
    _TemplateDotCmdParser();
    void doParse(String);
    HtmlTemplateItem getTemplateItem();

private:
    HtmlTemplateItem item;
};

}

#endif
