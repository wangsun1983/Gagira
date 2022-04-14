#ifndef __GAGRIA_HTML_TEMPLATE_REFER_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_REFER_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "TemplateCmdParser.hpp"
#include "HtmlTemplateReferItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateReferCmdParser) IMPLEMENTS(TemplateCmdParser){
public:
    _TemplateReferCmdParser();
    void doParse(String);
    HtmlTemplateItem getTemplateItem();

private:
    HtmlTemplateReferItem item;
};

}

#endif
