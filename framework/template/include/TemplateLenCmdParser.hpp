#ifndef __GAGRIA_HTML_TEMPLATE_LEN_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_LEN_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "TemplateCmdParser.hpp"
#include "HtmlTemplateLenItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateLenCmdParser) IMPLEMENTS(TemplateCmdParser){
public:
    _TemplateLenCmdParser();
    void doParse(String);
    HtmlTemplateItem getTemplateItem();

private:
    HtmlTemplateLenItem item;
};

}

#endif
