#include "TemplateConditionItem.hpp"
#include "ForEveryOne.hpp"
#include "TemplateParserDispatcher.hpp"

using namespace obotcha;

namespace gagira {

//---- TemplateConditionFactor ----
_TemplateConditionFactor::_TemplateConditionFactor() {
    mActions = ArrayList<TemplateItem>::New();
}

void _TemplateConditionFactor::setCondition(TemplateItem c) {
    mCondition = c;
}

TemplateItem _TemplateConditionFactor::getCondition() {
    return mCondition;
}

void _TemplateConditionFactor::addAction(TemplateItem item) {
    mActions->add(item);
}

TemplateScopedValue _TemplateConditionFactor::execute(TemplateScopedValueContainer container,Object obj) {
    auto result = mCondition->execute(container,obj);
    if(result->getBoolValue()) {
        String result = String::New("");
        ForEveryOne(action,mActions) {
            auto exec_result = action->execute(container,obj);
            if(exec_result != nullptr) {
                result = result->append(exec_result->toString());
            }
        }

        return TemplateScopedValue::New(result);
    }
    return nullptr;
}

//---- TemplateConditionFactor ----
_TemplateConditionItem::_TemplateConditionItem(ArrayList<TemplateConditionFactor>factors,ArrayList<TemplateItem> finalActions) {
    mFactors = factors;
    mFinalActions = finalActions;
}
 

TemplateScopedValue _TemplateConditionItem::execute(TemplateScopedValueContainer scopedValueContainer,TemplateObjectContainer objContainer) {
    auto obj = objContainer->getCurrent();
    scopedValueContainer->createNew();

#ifdef DEBUG_TEMPLATE_ITEM_COMMAND
    printf("[Execute Instruction]:[%s]\n",mCmd->toChars());
#endif

    ForEveryOne(factor,mFactors) {
        auto result = factor->execute(scopedValueContainer,obj);
        if(result != nullptr) {
            scopedValueContainer->removeCurrent();
            return result;
        }
    }

    if(mFinalActions->size() != 0) {
        String result = String::New("");
        ForEveryOne(action,mFinalActions) {
            auto exec_result = action->execute(scopedValueContainer,obj);
            if(exec_result != nullptr) {
                result = result->append(exec_result->toString());
            }
        }
        scopedValueContainer->removeCurrent();
        return TemplateScopedValue::New(result); 
    }
    scopedValueContainer->removeCurrent();
    return nullptr;
}

void _TemplateConditionItem::addContition(TemplateItem item) {
    //mIfConditions->add(item);
}

}
