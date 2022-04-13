#ifndef __GAGRIA_HTML_TEMPLATE_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateCmdParser) {
public:
    virtual void doParse(String) = 0;
    virtual HtmlTemplateItem getTemplateItem() = 0;
    virtual void addPrevContent(String){};
};

}

#endif
