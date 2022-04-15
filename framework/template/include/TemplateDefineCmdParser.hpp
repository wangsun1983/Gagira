#ifndef __GAGRIA_HTML_TEMPLATE_DEFINE_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_DEFINE_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "HtmlTemplate.hpp"
#include "HtmlTemplateDefineItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateDefineCmdParser) IMPLEMENTS(TemplateCmdParser){
public:
    _TemplateDefineCmdParser();
    void doParse(String);
    HtmlTemplateItem getTemplateItem();

private:
    HtmlTemplateDefineItem item;
};

}

#endif
