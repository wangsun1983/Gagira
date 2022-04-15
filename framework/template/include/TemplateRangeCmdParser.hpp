#ifndef __GAGRIA_HTML_TEMPLATE_RANGE_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_RANGE_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateRangeItem.hpp"
#include "TemplateCmdParser.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateRangeCmdParser) IMPLEMENTS(TemplateCmdParser){
public:
    _TemplateRangeCmdParser();
    void doParse(String);
    HtmlTemplateItem getTemplateItem();

private:
    HtmlTemplateRangeItem item;
};

}

#endif
