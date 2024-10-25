#include "TemplateLetItem.hpp"
#include "TemplateFunctionParser.hpp"

using namespace obotcha;

namespace gagira {

TemplateScopedValue _TemplateLetItem::execute(HashMap<String,TemplateScopedValue> scopedValues,Object obj) {
    return mAssignment->execute(scopedValues,obj);
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

