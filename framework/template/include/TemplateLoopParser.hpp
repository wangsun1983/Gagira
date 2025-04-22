#ifndef __GAGRIA_TEMPLATE_LOOP_PARSER_HPP__
#define __GAGRIA_TEMPLATE_LOOP_PARSER_HPP__

#include "TemplateParser.hpp"
#include "TemplateLoopItem.hpp"
#include "TemplateInstruction.hpp"
#include "LinkedList.hpp"

using namespace obotcha;

namespace gagira {

class _TemplateLoopParser;
class _TemplateConditionParser;

DECLARE_CLASS(TemplateLoopInnerParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    friend class _TemplateLoopParser;
    friend class _TemplateConditionParser;
    
    _TemplateLoopInnerParser(String cmd);
    TemplateItem doParse();
    void inject(String);
    bool processText(String);
    
private:
    enum Status {
        ForStatement = 0,
        FuncStatement
    };

    static const String AtString;
    ArrayList<TemplateItem> parseInitActions(String);
    ArrayList<TemplateItem> parseLoopActions(String);

    //ArrayList<String> parseStatement(String tag,char);
    int findInstruction(String cmd,String instruction);
    String mCmd;
    bool isEnd;
    TemplateLoopItem mForItem;
    LinkedList<TemplateParser> mParsers;
    int mStatus;
};

DECLARE_CLASS(TemplateLoopParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateLoopParser(String cmd);
    TemplateItem doParse();
    void inject(String);
    bool processText(String);
    
private:
    String mCmd;
    LinkedList<TemplateParser> mParsers;
};

}

#endif
