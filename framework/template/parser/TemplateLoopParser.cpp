#include "TemplateLoopParser.hpp"
#include "TemplateParserDispatcher.hpp"
#include "TemplateLetParser.hpp"
#include "TemplateFunctionParser.hpp"
#include "ForEveryOne.hpp"
#include "Log.hpp"
#include "TemplateParserHelper.hpp"
#include "TemplateConditionParser.hpp"

using namespace obotcha;

namespace gagira {

const String _TemplateLoopInnerParser::AtString = String::New("in");

_TemplateLoopInnerParser::_TemplateLoopInnerParser(String cmd) {
    mCmd = cmd;
    mForItem = TemplateLoopItem::New();
    isEnd = false;
    mType = Loop;
    mParsers = LinkedList<TemplateParser>::New();
    mStatus = Status::ForStatement;
}

TemplateItem _TemplateLoopInnerParser::doParse() {
    switch(mStatus) {
        case Status::ForStatement: {
            printf("#####mCmd is %s \n",mCmd->toChars());
            ArrayList<String> statements = st(TemplateParserHelper)::ParseStatement(mCmd,';');
            if(statements->size() == 1) {
                int index = findInstruction(mCmd,AtString);
                if(index == -1) {
                    LOG(ERROR)<<"TemplateLoopInnerParser doParse index is -1";
                    return nullptr;
                }
                String variable_str = mCmd->subString(0,index)->trim();
                String range_str = mCmd->subString(index + AtString->size(),mCmd->size() - index - AtString->size())->trim();

                if(variable_str->contains(",")) {
                    ArrayList<String> variable_names = variable_str->split(",");
                    mForItem->setVariableName1(variable_names->get(0)->trim());
                    mForItem->setVariablename2(variable_names->get(1)->trim());
                } else {
                    mForItem->setVariableName1(variable_str);
                }
                //take range name
                mForItem->setRangeName(range_str->trim());
                mStatus = FuncStatement;
            } else {
                mForItem->setLoopInitItems(parseInitActions(statements->get(0)));

                //setLoopConditionItem
                auto functionParser = TemplateFunctionParser::New(statements->get(1));
                mForItem->setLoopConditionItem(functionParser->doParse());

                //setLoopAction
                auto actionParser = TemplateFunctionParser::New(statements->get(2));
                mForItem->setLoopActionItems(parseLoopActions(statements->get(2)));
                mStatus = FuncStatement;
            }
        } break;

        case Status::FuncStatement: {          
            auto parser = st(TemplateParserDispatcher)::Apply(mCmd);
            switch(parser->getType()) {
                case Type::Loop:
                    //TODO
                break;

                case Type::Condition:
                    //TODO
                break;

                case Type::End:
                    return mForItem;
                break;

                default:
                    mForItem->addAction(parser->doParse());
                break;
            }
        } break;
    }
    
    return nullptr;
}

void _TemplateLoopInnerParser::inject(String cmd) {
    mCmd = cmd;
}

bool _TemplateLoopInnerParser::processText(String text) {
    if(mParsers->size() > 0) {
        mParsers->peekLast()->processText(text);
    }
    return true;
}

ArrayList<TemplateItem> _TemplateLoopInnerParser::parseInitActions(String cmd) {
    auto initStr = cmd->trim();
    printf("####parseInitActions,cmd is %s,this is %lx \n",cmd->toChars(),this);

    auto statements = st(TemplateParserHelper)::ParseStatement(initStr,',');
    auto initActions = ArrayList<TemplateItem>::New();

    ForEveryOne(str,statements) {
        printf("####add action start,str is %s \n",str->toChars());
        auto parser = TemplateLetParser::New(str);
        initActions->add(parser->doParse());
    }

    return initActions;
}

ArrayList<TemplateItem> _TemplateLoopInnerParser::parseLoopActions(String cmd) {
    auto loopActionStr = cmd->trim();
    auto statements = st(TemplateParserHelper)::ParseStatement(loopActionStr,',');
    auto loopActions = ArrayList<TemplateItem>::New();

    ForEveryOne(str,statements) {
        auto parser = TemplateFunctionParser::New(str);
        loopActions->add(parser->doParse());
    }

    return loopActions;
}

int _TemplateLoopInnerParser::findInstruction(String cmd,String instruction) {
    int hitCount = instruction->size();
    auto cmdStr = cmd->toChars();
    auto instructionStr = instruction->toChars();

    int cmdSize = cmd->size();
    int cmdStart = 0;
    int instructionStart = 0;
    for(;cmdStart < cmdSize;cmdStart++) {
        if(cmdStr[cmdStart] == '\\') {
            cmdStart++; //jump 2 byte
            instructionStart = 0;
            hitCount = instruction->size();
            continue;
        }

        if(cmdStr[cmdStart] == instructionStr[instructionStart]) {
            instructionStart++;
            hitCount--;
            if(hitCount == 0) {
                return cmdStart - instruction->size() + 1;
            }
            continue;
        }

        if(instructionStart != 0) {
            instructionStart = 0;
            hitCount = instruction->size();
        }
    }
    return -1;
}

//---------- TemplateLoopParser --------------
_TemplateLoopParser::_TemplateLoopParser(String cmd) {
    mParsers = LinkedList<TemplateParser>::New();
    auto innerParser = TemplateLoopInnerParser::New(cmd);
    mParsers->putLast(innerParser);
    mCmd = cmd;
    mType = Loop;
}

TemplateItem _TemplateLoopParser::doParse() {
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

void _TemplateLoopParser::inject(String v) {
    v = v->trim();
    printf("loop parser inject :%s \n",v->toChars());

    if (v->startsWith(IfCommand)) {
        auto innerParser = TemplateConditionInnerParser::New(v->subString(IfCommand->size(),v->size() - IfCommand->size()));
        mParsers->putLast(innerParser);
    } else if(v->startsWith(LoopCommand)) {
        printf("loop parser inject trace1 \n");
        auto innerParser = TemplateLoopInnerParser::New(v->subString(LoopCommand->size(),v->size() - LoopCommand->size()));
        mParsers->putLast(innerParser);
    } else {
        mParsers->peekLast()->inject(v);
    }
}

bool _TemplateLoopParser::processText(String text) {
    return mParsers->peekLast()->processText(text);
}
    
}


