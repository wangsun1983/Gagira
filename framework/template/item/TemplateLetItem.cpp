#include "TemplateLetItem.hpp"
#include "TemplateFunctionParser.hpp"

using namespace obotcha;

namespace gagira {

TemplateScopedValue _TemplateLetItem::execute(TemplateScopedValueContainer scopedvalueContainer,TemplateObjectContainer objContainer) {
    printf("TemplateLetItem execute\n");
    auto value = mAssignment->execute(scopedvalueContainer,objContainer)->toString();

    switch(mType) {
        case st(TemplateScopedValue)::Type::Integer:
            scopedvalueContainer->addScopedValue(mVariableName,TemplateScopedValue::New(value->toInteger()));
        break;

        case st(TemplateScopedValue)::Type::Double:
            scopedvalueContainer->addScopedValue(mVariableName,TemplateScopedValue::New(value->toDouble()));
        break;

        case st(TemplateScopedValue)::Type::String:
            scopedvalueContainer->addScopedValue(mVariableName,TemplateScopedValue::New(value));
        break;

        case st(TemplateScopedValue)::Type::Bool:
            scopedvalueContainer->addScopedValue(mVariableName,TemplateScopedValue::New(value->toBoolean()));
        break;
    }

    return nullptr;
}

void _TemplateLetItem::setVariableName(String name) {
    mVariableName = name;
}

String _TemplateLetItem::getVariableName() {
    return mVariableName;
}

void _TemplateLetItem::setType(int type) {
    mType = type;
}

int _TemplateLetItem::getType() {
    return mType;
}

void _TemplateLetItem::setAssignment(String cmds) {
    TemplateFunctionParser parser = TemplateFunctionParser::New(cmds);
    mAssignment = parser->doParse();
}

// TemplateItem _TemplateLetItem::getAssignment() {
//     return mAssignment;
// }

}

