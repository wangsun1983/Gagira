#ifndef __GAGRIA_HTML_TEMPLATE_HPP__
#define __GAGRIA_HTML_TEMPLATE_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "ArrayList.hpp"
#include "OStdReturnValue.hpp"
#include "TemplateConditionCmdParser.hpp"
#include "TemplateCmdParser.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HtmlTemplate) {
public:
    _HtmlTemplate();
    int import(String,int start = 0,bool onesection = false);
    String execute(Object data);

private:
    static String RangeCommand;
    static String IndexCommand;
    static String WithCommand;
    static String EndCommand;
    static String DotCommand;
    static String IfCommand;
    static String ElseIfCommand;
    static String ElseCommand;

    enum ParseStatus {
        ParseTag = 0,
        ParseIfContent,
        ParseElseIfContent,
        ParseElseContent
    };

    int doParse(String);
    
    ArrayList<HtmlTemplateItem> items;
    //TemplateConditionCmdParser conditionParser;
    TemplateCmdParser mCurrentParser;

    int tagStartIndex;
    int tagEndIndex;
    int mStatus;

};

}

#endif
