#include "TemplateParser.hpp"
#include "TemplateCommentItem.hpp"
#include "TemplateConditionParser.hpp"
#include "TemplateFunctionParser.hpp"
#include "TemplateParserDispatcher.hpp"
#include "TemplateTextItem.hpp"
#include "TemplateLoopParser.hpp"

using namespace obotcha;

namespace gagira {

_TemplateConditionInnerParser::_TemplateConditionInnerParser(String cmd) {
    mAnalyseFactors = LinkedList<TemplateConditionFactor>::New();
    mConditionFactors = ArrayList<TemplateConditionFactor>::New();
    mFinalActors = ArrayList<TemplateItem>::New();

    auto firstFactor = TemplateConditionFactor::New();
    mAnalyseFactors->putLast(firstFactor);

    mStatus = ParseCondition;
    mCmd = cmd;
    mType = Condition;
}

bool _TemplateConditionInnerParser::processText(String text) {
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

TemplateItem _TemplateConditionInnerParser::doParse() {
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
                // if(parser->getType() == Type::Condition) {
                //     auto firstFactor = TemplateConditionFactor::New();
                //     mAnalyseFactors->putLast(firstFactor);
                //     mStatus = ParseCondition;
                //     continue;
                // } else 
                if(parser->getType() == Type::End) {
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
                    mConditionFactors->add(item);
                    is_loop = false;
                    break;
                } else if(parser->getType() == Type::Elif) {
                    auto nextFactor = TemplateConditionFactor::New();
                    auto item = mAnalyseFactors->takeLast();
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

void _TemplateConditionInnerParser::inject(String cmd) {
    mCmd = cmd->trim();
}

//TemplateConditionParser
_TemplateConditionParser::_TemplateConditionParser(String cmd) {
    mParsers = LinkedList<TemplateParser>::New();
    TemplateConditionInnerParser innerParser = TemplateConditionInnerParser::New(cmd);
    mParsers->putLast(innerParser);
    mCmd = cmd;
    mType = Condition;
}
    
void _TemplateConditionParser::inject(String v) {
    v = v->trim();
    if (v->startsWith(IfCommand)) {
        auto innerParser = TemplateConditionInnerParser::New(v->subString(IfCommand->size(),v->size() - IfCommand->size()));
        mParsers->putLast(innerParser);
    } else if(v->startsWith(LoopCommand)) {
        auto innerParser = TemplateLoopInnerParser::New(v->subString(LoopCommand->size(),v->size() - LoopCommand->size()));
        mParsers->putLast(innerParser);
    } else {
        mParsers->peekLast()->inject(v);
    }
}

TemplateItem _TemplateConditionParser::doParse() {
    auto result = mParsers->peekLast()->doParse();
    if(result != nullptr) {
        auto current = mParsers->takeLast();
        if(mParsers->size() != 0) {
            auto prev = mParsers->peekLast();
            switch(prev->getType()) {
                case st(TemplateParser)::Type::Condition: {
                    auto prev_parser = Cast<TemplateConditionInnerParser>(prev);
                    prev_parser->mAnalyseFactors->peekLast()->addAction(result);
                } break;

                case st(TemplateParser)::Type::Loop: {
                    printf("add action to prev_parser \n");
                    auto prev_parser = Cast<TemplateLoopInnerParser>(prev);
                    prev_parser->mForItem->addAction(result);
                } break;
            }
        } else {
            return result;
        }
    }

    return nullptr;
}

bool _TemplateConditionParser::processText(String text) {
    return mParsers->peekLast()->processText(text);
}

}
