#include "TemplateLoopParser.hpp"
#include "TemplateParserDispatcher.hpp"

using namespace obotcha;

namespace gagira {

const String _TemplateLoopParser::AtString = String::New("in");

_TemplateLoopParser::_TemplateLoopParser(String cmd) {
    mCmd = cmd;
    mForItem = TemplateLoopItem::New();
    isEnd = false;
    mType = Loop;
    mParsers = LinkedList<TemplateParser>::New();
    mStatus = Status::ForStatement;
}

TemplateItem _TemplateLoopParser::doParse() {
    //set variable name
    //for x,y at .list ->take x,y first
    printf("TemplateLoopParser do parse trace1,mStatus is %d,mCmd is %s \n",mStatus,mCmd->toChars());
    switch(mStatus) {
        case Status::ForStatement: {
            int index = mCmd->indexOf(AtString);
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
            printf("TemplateLoopParser do parse trace2,rangeName is %s \n",range_str->toChars());
            mForItem->setRangeName(range_str->trim());
            mStatus = FuncStatement;
        } break;

        case Status::FuncStatement: {          
            auto parser = st(TemplateParserDispatcher)::Apply(mCmd);
            printf("parser type is %d \n",parser->getType());
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
                    printf("add for time!!!!!!! \n");
                    mForItem->addAction(parser->doParse());
                break;
            }
        } break;
    }
    
    return nullptr;
}

void _TemplateLoopParser::inject(String cmd) {
    mCmd = cmd;
}

bool _TemplateLoopParser::processText(String text) {
    printf("TemplateLoopParser,processText is %s \n",text->toChars());
    if(mParsers->size() > 0) {
        mParsers->peekLast()->processText(text);
    }
    return true;
}
    
}


