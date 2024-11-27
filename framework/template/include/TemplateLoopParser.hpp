#ifndef __GAGRIA_TEMPLATE_LOOP_PARSER_HPP__
#define __GAGRIA_TEMPLATE_LOOP_PARSER_HPP__

#include "TemplateParser.hpp"
#include "TemplateLoopItem.hpp"
#include "TemplateInstruction.hpp"
#include "LinkedList.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateLoopParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateLoopParser(String cmd);
    TemplateItem doParse();
    void inject(String);
    bool processText(String);
    
private:
    enum Status {
        ForStatement = 0,
        FuncStatement
    };

    static const String AtString;
    String mCmd;
    bool isEnd;
    TemplateLoopItem mForItem;
    LinkedList<TemplateParser> mParsers;
    int mStatus;
};

}

#endif
