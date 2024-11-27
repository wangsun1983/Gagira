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
    
    ArrayList<String> scopedItems = mCmd->split(EqualOperator);
    /**
     * sample:let int v = xxxxxxx
     *        ---------   -------
     *            |          |
     *            |          ---->valueText
     *            |
     *            ----> assignmentText
     */

    //start parse assignmentText
    auto assignmentText = scopedItems->get(0);
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
    auto valueText = scopedItems->get(1)->trim();
    item->setAssignment(valueText);
    return item;
}

}

