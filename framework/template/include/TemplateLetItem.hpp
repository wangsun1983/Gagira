#ifndef __GAGRIA_TEMPLATE_LET_ITEM_HPP__
#define __GAGRIA_TEMPLATE_LET_ITEM_HPP__

#include "TemplateItem.hpp"
#include "TemplateScopedValue.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateLetItem) IMPLEMENTS(TemplateItem){
public:
    TemplateScopedValue execute(HashMap<String,TemplateScopedValue> scopedValues,Object obj);
    void setVariableName(String);
    String getVariableName();

    void setType(int);
    int getType();

    void setAssignment(String);
    TemplateItem getAssignment();

private:
    String mVariableName;
    int type;

    TemplateItem mAssignment;
};

}

#endif
