#ifndef __GAGRIA_HTML_TEMPLATE_INDEX_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_INDEX_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "TemplateCmdParser.hpp"
#include "HtmlTemplateIndexItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateIndexCmdParser) IMPLEMENTS(TemplateCmdParser){
public:
    _TemplateIndexCmdParser();
    void doParse(String);
    HtmlTemplateItem getTemplateItem();

private:
    HtmlTemplateIndexItem item;
    bool isDigit(unsigned char);
};

}

#endif
