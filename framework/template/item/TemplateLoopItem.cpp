#include "TemplateLoopItem.hpp"
#include "Reflect.hpp"
#include "Field.hpp"
#include "OStdInstanceOf.hpp"
#include "ForEveryOne.hpp"
#include "IllegalArgumentException.hpp"

namespace gagira {

_TemplateLoopItem::_TemplateLoopItem() {
    mItems = ArrayList<TemplateItem>::New();
}

void _TemplateLoopItem::addAction(TemplateItem item) {
    mItems->add(item);
}

TemplateScopedValue _TemplateLoopItem::execute(TemplateScopedValueContainer scopedContainer,TemplateObjectContainer objectContainer) {
    //first get object
    printf("mRangeName is %s \n",mRangeName->toChars());
    //TODO
    auto field = objectContainer->getCurrent()->getField(mRangeName->subString(1,mRangeName->size() - 1));
    switch(field->getType()) {
        case st(Field)::Type::ArrayList: {
            auto size = field->getContainerSize();
            for(int i = 0;i < size;i++) {
                scopedContainer->createNew();
                auto obj = field->getListItemObject(i);
                if(obj == nullptr) {
                    printf("i[%d] get a null value!! \n",i);
                }
                printf("--- add variableName1 is [%s],mItems size is %d--- \n",mForVariableName1->toChars(),mItems->size());
                objectContainer->setObjInLifeCycle(mForVariableName1,obj);
                ForEveryOne(item,mItems) {
                    item->execute(scopedContainer,objectContainer);
                }
                printf("---finish one item --- \n");
                objectContainer->removeObjInLifeCycle(mForVariableName1);
            }
        } break;

        case st(Field)::Type::HashMap: {
            auto pairs = field->getMapItemObjects();
            ForEveryOne(p,pairs) {
                scopedContainer->createNew();
                auto key_obj = p->getKey();
                auto value_obj = p->getValue();
                updateScopedValues(scopedContainer,mForVariableName1,key_obj);
                updateScopedValues(scopedContainer,mForVariableName2,value_obj);
                
                ForEveryOne(item,mItems) {
                    item->execute(scopedContainer,objectContainer);
                }
                scopedContainer->removeCurrent();
            }
        } break;
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
