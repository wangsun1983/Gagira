#include "TemplateLetParser.hpp"
#include "TemplateLetItem.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

_TemplateLetParser::_TemplateLetParser(String cmd) {
    mCmd = cmd;
    mStatus = ParseType;
    mType = Let;
}

TemplateItem _TemplateLetParser::doParse() {
    TemplateLetItem item = TemplateLetItem::New();
    printf("TemplateLetParser doParse cmd is %s \n",mCmd->toChars());
    //ArrayList<String> scopedItems = mCmd->split(EqualOperator);
    auto index = mCmd->indexOf(EqualOperator);
    if(index == -1) {
        return nullptr;
    }

    /**
     * sample:let int v = xxxxxxx
     *        ---------   -------
     *            |          |
     *            |          ---->valueText
     *            |
     *            ----> assignmentText
     */

    //start parse assignmentText
    auto assignmentText = mCmd->subString(0,index);
    ArrayList<String> assignmentItems = assignmentText->split(SpaceTextSymbol);
    ForEveryOne(assignment,assignmentItems) {
        if(assignment->sameAs(SpaceTextSymbol)) {
            continue;
        }

        switch(mStatus) {
            case ParseType: {
                if(assignment->sameAs(IntVariableType)) {
                    mParsedType = st(TemplateScopedValue)::Type::Integer;
                } else if(assignment->sameAs(DoubleVariableType)) {
                    mParsedType = st(TemplateScopedValue)::Type::Double;
                } else if(assignment->sameAs(StringVariableType)) {
                    mParsedType = st(TemplateScopedValue)::Type::String;
                } else if(assignment->sameAs(BoolVariableType)) {
                    mParsedType = st(TemplateScopedValue)::Type::Bool;
                }
                mStatus = ParseScopedName;
            } break;

            case ParseScopedName: {
                mParsedName = assignment;
                mStatus = ParseScopdeValue;
            } break;
        }
    }

    item->setVariableName(mParsedName);
    item->setType(mParsedType);

    //start parse valueText
    auto valueText = mCmd->subString(index + 1,mCmd->size() - index - 1);
    item->setAssignment(valueText);
    return item;
}

}

