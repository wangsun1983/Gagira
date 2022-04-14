#include "TemplateConditionCmdParser.hpp"
#include "HtmlTemplateObjectItem.hpp"

using namespace obotcha;

namespace gagira {

const String _TemplateConditionCmdParser::Eq = createString("eq");
const String _TemplateConditionCmdParser::Ne = createString("ne");
const String _TemplateConditionCmdParser::Lt = createString("lt");
const String _TemplateConditionCmdParser::Le = createString("le");
const String _TemplateConditionCmdParser::Gt = createString("gt");
const String _TemplateConditionCmdParser::Ge = createString("ge");


_TemplateConditionCmdParser::_TemplateConditionCmdParser() {
    mDotCmdParser = createTemplateDotCmdParser();
    item = createHtmlTemplateConditionItem();
}

//void _TemplateConditionCmdParser::addPrevContent(String content) {
//    auto condition =  item->conditions->get(item->conditions->size() - 1);
//    condition->content = content;
//}

HtmlTemplateItem _TemplateConditionCmdParser::getTemplateItem() {
    return item;
}

void _TemplateConditionCmdParser::addFinalItem() {
    HtmlTemplateCondition cond = createHtmlTemplateCondition();
    cond->isFinalCondition = true;
    item->conditions->add(cond);
}

void _TemplateConditionCmdParser::doParse(String command) {
    printf("TemplateConditionCmdParser command is %s \n",command->toChars());
    ArrayList<String> strs = command->split(" ");
    printf("TemplateConditionCmdParser strs size is %d \n",strs->size());

    HtmlTemplateCondition cond = createHtmlTemplateCondition();
    
    auto iterator = strs->getIterator();
    int internalStatus = ParseAction;

    while(iterator->hasValue()) {
        String v = iterator->getValue();
        
        if(v->size() == 0 || v->equals(" ")) {
            iterator->next();
            continue;
        }

        switch(internalStatus) {
            case ParseAction:
                printf("ParseAction is %s \n",v->toChars());
                if (v->equalsIgnoreCase(Eq) || v->equalsIgnoreCase(Ne) ||
                    v->equalsIgnoreCase(Lt) || v->equalsIgnoreCase(Le) ||
                    v->equalsIgnoreCase(Gt) || v->equalsIgnoreCase(Ge)) {
                    cond->condition = v;
                    internalStatus = ParseValue1;
                } else if (v->startsWith(".")) {
                    printf("ParseAction,set uniqueItem \n");
                    mDotCmdParser->doParse(v->subString(1, v->size() - 1));
                    cond->uniqueItem = Cast<HtmlTemplateObjectItem>(
                        mDotCmdParser->getTemplateItem());
                    internalStatus = ParseEnd;
                }
            break;

            case ParseValue1:
                if(v->startsWith(".")) {
                    mDotCmdParser->doParse(v->subString(1,v->size() - 1));
                    cond->objItem1 = Cast<HtmlTemplateObjectItem>(mDotCmdParser->getTemplateItem());
                } else {
                    cond->stringItem1 = v;
                }
                internalStatus = ParseValue2;
            break;

            case ParseValue2:
                if(v->startsWith(".")) {
                    mDotCmdParser->doParse(v->subString(1,v->size() - 1));
                    cond->objItem2 = Cast<HtmlTemplateObjectItem>(mDotCmdParser->getTemplateItem());
                } else {
                    cond->stringItem2 = v;
                }
                internalStatus = ParseEnd;
            break;
        }

        if(internalStatus == ParseEnd) {
            break;
        }

        iterator->next();
    }

    item->conditions->add(cond);
}


}
