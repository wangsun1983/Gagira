#ifndef __GAGRIA_TEMPLATE_LOOP_ITEM_HPP__
#define __GAGRIA_TEMPLATE_LOOP_ITEM_HPP__

#include "TemplateItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateLoopItem) IMPLEMENTS(TemplateItem){
public:
    _TemplateLoopItem();
    void setVariableName1(String);
    void setVariablename2(String);
    void setRangeName(String);

    String getVariableName1();
    String getVariableName2();
    String getRangeName();
    
    void addAction(TemplateItem);
    TemplateScopedValue execute(TemplateScopedValueContainer ,TemplateObjectContainer);

private:
    String mForVariableName1;
    String mForVariableName2;
    String mRangeName;
    ArrayList<TemplateItem> mItems;
    bool isEnd;
    void updateScopedValues(TemplateScopedValueContainer,String variableName,Object);
};

}

#endif
