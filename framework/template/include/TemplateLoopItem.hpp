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

    void setLoopConditionItem(TemplateItem);
    void setLoopActionItems(ArrayList<TemplateItem>);
    void setLoopInitItems(ArrayList<TemplateItem>);

    TemplateItem getLoopConditionItem();
    ArrayList<TemplateItem> getLoopActionItems();
    ArrayList<TemplateItem> getLoopInitItems();
    
    void addAction(TemplateItem);
    TemplateScopedValue execute(TemplateScopedValueContainer ,TemplateObjectContainer);

private:

    /*------------- loop params -------------*/
    //the following three params is used for
    //loop x in .list
    String mForVariableName1;
    String mForVariableName2;
    String mRangeName;

    //the follow
    //loop int i = 1,j = 2;i + j < 100;i++,j++
    TemplateItem mLoopConditionItem;
    ArrayList<TemplateItem> mLoopActionItems;
    ArrayList<TemplateItem> mInitItems;
    /*------------- loop params -------------*/

    ArrayList<TemplateItem> mItems;
    bool isEnd;
    void updateScopedValues(TemplateScopedValueContainer,String variableName,Object);
};

}

#endif
