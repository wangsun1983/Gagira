#include "TemplateLetItem.hpp"
#include "TemplateFunctionParser.hpp"

using namespace obotcha;

namespace gagira {

TemplateScopedValue _TemplateLetItem::execute(TemplateScopedValueContainer scopedvalueContainer,TemplateObjectContainer objContainer) {
    auto obj = objContainer->getCurrent();
    auto value = mAssignment->execute(scopedvalueContainer,obj);
    scopedvalueContainer->addScopedValue(mVariableName,value);
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

TemplateItem _TemplateLetItem::getAssignment() {
    return mAssignment;
}

}

