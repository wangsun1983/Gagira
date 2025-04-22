#include "TemplateLoopItem.hpp"
#include "Reflect.hpp"
#include "Field.hpp"
#include "OStdInstanceOf.hpp"
#include "ForEveryOne.hpp"
#include "IllegalArgumentException.hpp"

namespace gagira {

_TemplateLoopItem::_TemplateLoopItem() {
    mItems = ArrayList<TemplateItem>::New();
    mForVariableName1 = nullptr;
    mForVariableName2 = nullptr;
    mRangeName = nullptr;
    
    mLoopConditionItem = nullptr;
    mLoopActionItems = ArrayList<TemplateItem>::New();
    mInitItems = ArrayList<TemplateItem>::New();
}

void _TemplateLoopItem::addAction(TemplateItem item) {
    mItems->add(item);
}

TemplateScopedValue _TemplateLoopItem::execute(TemplateScopedValueContainer scopedContainer,TemplateObjectContainer objectContainer) {
    //first get object
    if(mForVariableName1 != nullptr || mForVariableName1 != nullptr) {
        auto field = objectContainer->getCurrent()->getField(mRangeName->subString(1,mRangeName->size() - 1));
        switch(field->getType()) {
            case st(Field)::Type::ArrayList: {
                auto size = field->getContainerSize();
                for(int i = 0;i < size;i++) {
                    scopedContainer->createNew();
                    auto obj = field->getListItemObject(i);
                    objectContainer->setObjInLifeCycle(mForVariableName1,obj);
                    bool isReturn = false;
                    TemplateScopedValue returnValue = nullptr;
                    ForEveryOne(item,mItems) {
                        returnValue = item->execute(scopedContainer,objectContainer);
                        if(returnValue->isDirectReturn()) {
                            isReturn = true;
                            break;
                        }
                    }
                    objectContainer->removeObjInLifeCycle(mForVariableName1);
                    scopedContainer->removeCurrent();
                    if(isReturn) {
                        return returnValue;
                    }
                }
            } break;

            case st(Field)::Type::HashMap: {
                auto pairs = field->getMapItemObjects();
                ForEveryOne(p,pairs) {
                    scopedContainer->createNew();
                    auto key_obj = p->getKey();
                    auto value_obj = p->getValue();
                    // updateScopedValues(scopedContainer,mForVariableName1,key_obj);
                    // updateScopedValues(scopedContainer,mForVariableName2,value_obj);
                    objectContainer->setObjInLifeCycle(mForVariableName1,key_obj);
                    objectContainer->setObjInLifeCycle(mForVariableName2,value_obj);
                    bool isReturn = false;
                    TemplateScopedValue returnValue = nullptr;

                    ForEveryOne(item,mItems) {
                        returnValue = item->execute(scopedContainer,objectContainer);
                        if(returnValue->isDirectReturn()) {
                            isReturn = true;
                            break;
                        }
                    }
                    objectContainer->removeObjInLifeCycle(mForVariableName1);
                    objectContainer->removeObjInLifeCycle(mForVariableName2);
                    scopedContainer->removeCurrent();
                    if(isReturn) {
                        return returnValue;
                    }
                }
            } break;
        }
    } else {
        scopedContainer->createNew();
        printf("------- start initItem ----------\n");
        ForEveryOne(initItem,mInitItems) {
            initItem->execute(scopedContainer,objectContainer);
        }
        printf("------- finish initItem ----------\n");

        bool isReturn = false;
        bool isBreak = false;
        TemplateScopedValue returnValue = nullptr;
        while(1) {
            auto scopedValue = mLoopConditionItem->execute(scopedContainer,objectContainer);
            if(!scopedValue->getBoolValue()) {
                break;
            }
            
            ForEveryOne(item,mItems) {
                returnValue = item->execute(scopedContainer,objectContainer);
                if(returnValue != nullptr) {
                    if(returnValue->isDirectReturn()) {
                        isReturn = true;
                        break;
                    } else if(returnValue->isDirectBreak()) {
                        isBreak = true;
                        break;
                    }
                }
            }

            if(!isReturn && !isBreak) {
                ForEveryOne(action,mLoopActionItems) {
                    action->execute(scopedContainer,objectContainer);
                }
                continue;
            }

            break;
        }

        scopedContainer->removeCurrent();
        if(isReturn) {
            return returnValue;
        }
    }
    
    return nullptr;
}

void _TemplateLoopItem::setVariableName1(String name) {
    mForVariableName1 = name;
}

void _TemplateLoopItem::setVariablename2(String name) {
    mForVariableName2 = name;
}

void _TemplateLoopItem::setRangeName(String name) {
    mRangeName = name;
}

String _TemplateLoopItem::getVariableName1() {
    return mForVariableName1;
}

String _TemplateLoopItem::getVariableName2() {
    return mForVariableName2;
}

String _TemplateLoopItem::getRangeName() {
    return mRangeName;
}

void _TemplateLoopItem::setLoopConditionItem(TemplateItem item) {
    mLoopConditionItem = item;
}

void _TemplateLoopItem::setLoopActionItems(ArrayList<TemplateItem> items) {
    mLoopActionItems = items;
}

void _TemplateLoopItem::setLoopInitItems(ArrayList<TemplateItem> items) {
    mInitItems = items;
}

TemplateItem _TemplateLoopItem::getLoopConditionItem() {
    return mLoopConditionItem;
}

ArrayList<TemplateItem> _TemplateLoopItem::getLoopActionItems() {
    return mLoopActionItems;
}

ArrayList<TemplateItem> _TemplateLoopItem::getLoopInitItems() {
    return mInitItems;
}

void _TemplateLoopItem::updateScopedValues(TemplateScopedValueContainer container,
                                    String variableName,Object obj) {
    if(IsInstance(Integer,obj)) {
        container->addScopedValue(variableName,
                                        TemplateScopedValue::New(Cast<Integer>(obj)));
    } else if(IsInstance(String,obj)) {
        container->addScopedValue(variableName,
                                        TemplateScopedValue::New(Cast<String>(obj)));
    } else if(IsInstance(Boolean,obj)) {
        container->addScopedValue(variableName,
                                        TemplateScopedValue::New(Cast<Boolean>(obj)));
    } else if(IsInstance(Double,obj)) {
        container->addScopedValue(variableName,
                                        TemplateScopedValue::New(Cast<Double>(obj)));
    } else {
        Trigger(IllegalArgumentException,"fail to execute for looper");
    }
}

}
