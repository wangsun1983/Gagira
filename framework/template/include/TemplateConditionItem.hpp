#ifndef __GAGRIA_TEMPLATE_CONDITION_ITEM_HPP__
#define __GAGRIA_TEMPLATE_CONDITION_ITEM_HPP__

#include "TemplateItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateConditionFactor) {
public:
    _TemplateConditionFactor();
    void setCondition(TemplateItem);
    TemplateItem getCondition();

    void addAction(TemplateItem);
    void addActionString(String);
    
    TemplateScopedValue execute(TemplateScopedValueContainer,Object obj);

private:
    TemplateItem mCondition;
    ArrayList<TemplateItem> mActions;
};

DECLARE_CLASS(TemplateConditionItem) IMPLEMENTS(TemplateItem){
public:
    _TemplateConditionItem(ArrayList<TemplateConditionFactor> if_action,ArrayList<TemplateItem> final_action);

    TemplateScopedValue execute(TemplateScopedValueContainer ,TemplateObjectContainer);

    void addContition(TemplateItem);
private:
    ArrayList<TemplateConditionFactor> mFactors;
    ArrayList<TemplateItem> mFinalActions; //else
};

}

#endif
