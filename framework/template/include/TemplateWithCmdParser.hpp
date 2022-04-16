#ifndef __GAGRIA_HTML_TEMPLATE_WITH_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_WITH_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "TemplateCmdParser.hpp"
#include "HtmlTemplateReferItem.hpp"
#include "HtmlTemplateWithItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateWithCmdParser) IMPLEMENTS(TemplateCmdParser){
public:
    _TemplateWithCmdParser();
    void doParse(String);
    HtmlTemplateItem getTemplateItem();

private:
    HtmlTemplateWithItem item;
};

}

#endif
