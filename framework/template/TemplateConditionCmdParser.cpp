#include "TemplateConditionCmdParser.hpp"
#include "HtmlTemplateObjectItem.hpp"

using namespace obotcha;

namespace gagira {

const String _TemplateConditionCmdParser::Eq = String::New("eq");
const String _TemplateConditionCmdParser::Ne = String::New("ne");
const String _TemplateConditionCmdParser::Lt = String::New("lt");
const String _TemplateConditionCmdParser::Le = String::New("le");
const String _TemplateConditionCmdParser::Gt = String::New("gt");
const String _TemplateConditionCmdParser::Ge = String::New("ge");


_TemplateConditionCmdParser::_TemplateConditionCmdParser() {
    mDotCmdParser = TemplateDotCmdParser::New();
    item = HtmlTemplateConditionItem::New();
}

HtmlTemplateItem _TemplateConditionCmdParser::getTemplateItem() {
    return item;
}

void _TemplateConditionCmdParser::addFinalItem() {
    HtmlTemplateCondition cond = HtmlTemplateCondition::New();
    cond->isFinalCondition = true;
    item->conditions->add(cond);
}

void _TemplateConditionCmdParser::doParse(String command) {
    ArrayList<String> strs = command->split(" ");
    
    HtmlTemplateCondition cond = HtmlTemplateCondition::New();
    
    auto iterator = strs->getIterator();
    int internalStatus = ParseAction;

    while(iterator->hasValue()) {
        String v = iterator->getValue();
        
        if(v->size() == 0 || v->sameAs(" ")) {
            iterator->next();
            continue;
        }

        switch(internalStatus) {
            case ParseAction:
                if (v->equalsIgnoreCase(Eq) || v->equalsIgnoreCase(Ne) ||
                    v->equalsIgnoreCase(Lt) || v->equalsIgnoreCase(Le) ||
                    v->equalsIgnoreCase(Gt) || v->equalsIgnoreCase(Ge)) {
                    cond->condition = v;
                    internalStatus = ParseValue1;
                } else if (v->startsWith(".")) {
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
