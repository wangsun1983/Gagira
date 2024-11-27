#include "TemplateParser.hpp"
#include "TemplateCommentItem.hpp"
#include "TemplateConditionParser.hpp"
#include "TemplateFunctionParser.hpp"
#include "TemplateParserDispatcher.hpp"
#include "TemplateTextItem.hpp"

using namespace obotcha;

namespace gagira {

_TemplateConditionParser::_TemplateConditionParser(String cmd) {
    mAnalyseFactors = LinkedList<TemplateConditionFactor>::New();
    mConditionFactors = ArrayList<TemplateConditionFactor>::New();
    mFinalActors = ArrayList<TemplateItem>::New();

    auto firstFactor = TemplateConditionFactor::New();
    mAnalyseFactors->putLast(firstFactor);

    mStatus = ParseCondition;
    mCmd = cmd;
    mType = Condition;
}

bool _TemplateConditionParser::processText(String text) {
    switch(mStatus) {
        case ParseFinalAction:
            mFinalActors->add(TemplateTextItem::New(text));
        break;

        case ParseAction:
            mAnalyseFactors->peekLast()->addAction(TemplateTextItem::New(text));
        break;
    }
    return true;
}

TemplateItem _TemplateConditionParser::doParse() {
    bool is_complete = false;
    bool is_loop = true;

    bool is_fi = false;
    while(is_loop) {
        switch(mStatus) {
            case ParseCondition: {
                auto first_left_brace = mCmd->indexOf(LeftBraceMark);
                auto last_right_brace = mCmd->indexOf(RightBraceMark);
                
                auto condition_content = mCmd->subString(first_left_brace + 1,last_right_brace - first_left_brace - 1);
                auto parser = TemplateFunctionParser::New(condition_content);
                mAnalyseFactors->peekLast()->setCondition(parser->doParse());
                mStatus = ParseAction;
                is_loop = false;
            } break;

            case ParseFinalAction:
            case ParseAction: {
                auto parser = st(TemplateParserDispatcher)::Apply(mCmd);
                if(parser->getType() == Type::Condition) {
                    auto firstFactor = TemplateConditionFactor::New();
                    mAnalyseFactors->putLast(firstFactor);
                    mStatus = ParseCondition;
                    continue;
                } else if(parser->getType() == Type::End) {
                    auto item = mAnalyseFactors->takeLast();
                    if(item != nullptr) {
                        mConditionFactors->add(item);
                    }
                    is_loop = false;
                    is_fi = true;
                    break;
                } else if(parser->getType() == Type::Else) {
                    mStatus = ParseFinalAction;
                    auto item = mAnalyseFactors->takeLast();
                    if(item == nullptr) {
                        printf("---------- trace1 item is null!!!!! \n");
                    }
                    mConditionFactors->add(item);
                    is_loop = false;
                    break;
                } else if(parser->getType() == Type::Elif) {
                    auto nextFactor = TemplateConditionFactor::New();
                    auto item = mAnalyseFactors->takeLast();
                    if(item == nullptr) {
                        printf("---------- trace2 item is null!!!!! \n");
                    }
                    mConditionFactors->add(item);
                    mAnalyseFactors->putLast(nextFactor);
                    mStatus = ParseCondition;
                    continue;
                }

                if(mStatus == ParseAction) {
                    mAnalyseFactors->peekLast()->addAction(parser->doParse());
                } else {
                    auto item = parser->doParse();
                    if(item != nullptr) {
                        mFinalActors->add(item);
                    }
                }

                is_loop = false;
            } break;
        }
    }

    if(!is_fi) {
        return nullptr;
    }

    auto item = TemplateConditionItem::New(mConditionFactors,mFinalActors);
#ifdef DEBUG_TEMPLATE_ITEM_COMMAND    
    item->setCmd(mCmd);
#endif
    return item;
}

void _TemplateConditionParser::inject(String cmd) {
    mCmd = cmd->trim();
}
    

}
