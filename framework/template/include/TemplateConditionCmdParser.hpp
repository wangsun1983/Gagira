#ifndef __GAGRIA_HTML_TEMPLATE_CONDITION_CMD_PARSER_HPP__
#define __GAGRIA_HTML_TEMPLATE_CONDITION_CMD_PARSER_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "HtmlTemplateConditionItem.hpp"
#include "TemplateCmdParser.hpp"
#include "TemplateDotCmdParser.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateConditionCmdParser) IMPLEMENTS(TemplateCmdParser) {
public:
    _TemplateConditionCmdParser();

//    void addPrevContent(String);
    void doParse(String);
    void addFinalItem();
    HtmlTemplateItem getTemplateItem();

private:
    enum InternalStatus {
        ParseAction = 0,
        ParseValue1,
        ParseValue2,
        ParseEnd
    };

    static const String Eq;      //如果arg1 == arg2则返回真
    static const String Ne;      //如果arg1 != arg2则返回真
    static const String Lt;      //如果arg1 < arg2则返回真
    static const String Le;      //如果arg1 <= arg2则返回真
    static const String Gt;      //如果arg1 > arg2则返回真
    static const String Ge;      //如果arg1 >= arg2则返回真

    int mStatus;
    HtmlTemplateConditionItem item;

    TemplateDotCmdParser mDotCmdParser;
    
    
};

}

#endif
