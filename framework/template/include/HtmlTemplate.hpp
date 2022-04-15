#ifndef __GAGRIA_HTML_TEMPLATE_HPP__
#define __GAGRIA_HTML_TEMPLATE_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "ArrayList.hpp"
#include "OStdReturnValue.hpp"
#include "TemplateConditionCmdParser.hpp"
#include "TemplateCmdParser.hpp"
#include "HashMap.hpp"
#include "File.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HtmlTemplate) {
public:
    _HtmlTemplate();
    int import(String,int start = 0,bool onesection = false);
    int importFile(String);
    String execute(Object data);
    String execute(String section,Object data);

private:
    static String RangeCommand;
    static String IndexCommand;
    static String WithCommand;
    static String EndCommand;
    static String DotCommand;
    static String IfCommand;
    static String ElseIfCommand;
    static String ElseCommand;
    static String IncludeCommand;
    static String DefineCommand;

    enum ParseStatus {
        ParseTag = 0,
        ParseIfContent,
        ParseElseIfContent,
        ParseElseContent,
        ParseRangeContent,
        ParseDefine,
    };

    int doParse(String);
    
    ArrayList<HtmlTemplateItem> items;

    HashMap<String,HtmlTemplateItem> sections;

    TemplateCmdParser mCurrentParser;

    int tagStartIndex;
    int tagEndIndex;
    int mStatus[32];
    int mStatusCount;

    int currentStatus();
    void addCurrentStatus(int);
    void removeCurrentStatus();

};

}

#endif
