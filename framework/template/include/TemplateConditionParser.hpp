#ifndef __GAGRIA_TEMPLATE_CONDITION_PARSER_HPP__
#define __GAGRIA_TEMPLATE_CONDITION_PARSER_HPP__

#include "TemplateParser.hpp"
#include "TemplateCommentItem.hpp"
#include "TemplateInstruction.hpp"
#include "TemplateConditionItem.hpp"
#include "LinkedList.hpp"

using namespace obotcha;

namespace gagira {

class _TemplateLoopParser;
class _TemplateConditionParser;

DECLARE_CLASS(TemplateConditionInnerParser) IMPLEMENTS(TemplateParser,TemplateInstruction) {
public:
    friend class _TemplateConditionParser;
    friend class _TemplateLoopParser;

    _TemplateConditionInnerParser(String cmd);
    void inject(String);
    TemplateItem doParse();
    bool processText(String);
    
private:
    enum Status {
        ParseCondition = 0,
        ParseAction,
        ParseFinalAction,
    };

    String mCmd;
    int mStatus;
    int mStart;
    int mCursor;

    LinkedList<TemplateConditionFactor> mAnalyseFactors;
    ArrayList<TemplateConditionFactor> mConditionFactors;
    ArrayList<TemplateItem> mFinalActors;
};

DECLARE_CLASS(TemplateConditionParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateConditionParser(String cmd);
    
    void inject(String);
    TemplateItem doParse();
    bool processText(String);
    
private:
    String mCmd;
    LinkedList<TemplateParser> mParsers;
};


}

#endif
