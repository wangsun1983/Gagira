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
#include "Utils.hpp"
#include "HtmlTemplateFunction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HtmlTemplate) {
public:
    _HtmlTemplate();
    int import(String,int start = 0,bool onesection = false);
    int importFile(String);
    String execute(Object data);
    String execute(String section,Object data);
    void setTemplateFunc(String,HtmlTemplateFunction);
    void saveFuncObjCache(Object);

    ArrayList<HtmlTemplateItem> getItems();

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
    static String LenCommand;

private:
    enum ParseStatus {
        ParseTag = 0,
        ParseIfContent,
        ParseElseIfContent,
        ParseElseContent,
        ParseRangeContent,
        ParseDefine,
        ParseWith,
    };

    int doParse(String);

    ArrayList<Object> functionsCache;
    std::map<std::string,HtmlTemplateFunction> mFunctions;
    
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

#define InjectTemplateFunction(templateObj,name,instance,method) \
    {\
        auto func = std::bind(&decltype(getClass(instance))::method,instance.get_pointer(),std::placeholders::_1);\
        templateObj->setTemplateFunc(name,func);\
        templateObj->saveFuncObjCache(instance);\
    }\

}

#endif
