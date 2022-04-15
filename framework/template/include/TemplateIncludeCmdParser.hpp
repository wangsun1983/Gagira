#ifndef __GAGRIA_HTML_TEMPLATE_INCLUDE_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_INCLUDE_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "TemplateCmdParser.hpp"
#include "HtmlTemplateIncludeItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateIncludeCmdParser) IMPLEMENTS(TemplateCmdParser){
public:
    _TemplateIncludeCmdParser();
    void doParse(String);
    HtmlTemplateItem getTemplateItem();

private:
    HtmlTemplateIncludeItem item;
};

}

#endif
