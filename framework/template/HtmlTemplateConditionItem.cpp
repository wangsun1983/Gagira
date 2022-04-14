#include "HtmlTemplateConditionItem.hpp"
#include "HtmlTemplateObjectItem.hpp"
#include "StringBuffer.hpp"
#include "HtmlTemplate.hpp"


namespace gagira {

_HtmlTemplateCondition::_HtmlTemplateCondition() {
    condition = nullptr;
    uniqueItem = nullptr;
    objItem1 = nullptr;
    objItem2 = nullptr;
    stringItem1 = nullptr;
    stringItem2 = nullptr;
    content = nullptr;
}

_HtmlTemplateCondition::~_HtmlTemplateCondition() {

}

_HtmlTemplateConditionItem::_HtmlTemplateConditionItem() {
    conditions = createArrayList<HtmlTemplateCondition>();
}

String _HtmlTemplateConditionItem::toString(Object o) {
    StringBuffer content = createStringBuffer();
    auto iterator = conditions->getIterator();
    printf("HtmlTemplateConditionItem condition size is %d \n",conditions->size());
    while(iterator->hasValue()) {
        HtmlTemplateCondition condition = iterator->getValue();
        if(condition->isFinalCondition) {
            return condition->content->execute(o);
        } else if(condition->uniqueItem != nullptr) {
            String value = condition->uniqueItem->toString(o);
            printf("unique item value is %s \n",value->toChars());
            try {
                int val = value->toBasicInt();
                printf("condition execute1!!! \n");
                if(val != 0) {
                    printf("condition execute1_1!!! \n");
                    return condition->content->execute(o);
                }
            } catch(...) {
                bool val = value->toBasicBool();
                printf("condition execute2!!! \n");
                if(val) {
                    printf("condition execute2_1!!! \n");
                    return condition->content->execute(o);
                }
            }
        } else {
            String param1 = nullptr;
            if(condition->objItem1 != nullptr) {
                param1 = condition->objItem1->toString(o);
            } else {
                param1 = condition->stringItem1;
            }

            String param2 = nullptr;
            if(condition->objItem2 != nullptr) {
                param2 = condition->objItem2->toString(o);
            } else {
                param2 = condition->stringItem2;
            }

            if(condition->condition->equalsIgnoreCase("eq")) {
                if(param1->equals(param2)) {
                    return condition->content->execute(o); 
                }
            } else if(condition->condition->equalsIgnoreCase("ne")) {
                if(!param1->equals(param2)) {
                    return condition->content->execute(o);
                }
            } else if(condition->condition->equalsIgnoreCase("lt")) {
                uint64_t v1 = param1->toBasicUint64();
                uint64_t v2 = param2->toBasicUint64();
                if(v1 < v2) {
                    return condition->content->execute(o);
                }
            } else if(condition->condition->equalsIgnoreCase("le")) {
                uint64_t v1 = param1->toBasicUint64();
                uint64_t v2 = param2->toBasicUint64();
                if(v1 <= v2) {
                    return condition->content->execute(o);
                }
            } else if(condition->condition->equalsIgnoreCase("gt")) {
                uint64_t v1 = param1->toBasicUint64();
                uint64_t v2 = param2->toBasicUint64();
                printf("v1 is %s,v2 is %s \n",param1->toChars(),param2->toChars());
                if(v1 > v2) {
                    return condition->content->execute(o);
                }
            } else if(condition->condition->equalsIgnoreCase("ge")) {
                uint64_t v1 = param1->toBasicUint64();
                uint64_t v2 = param2->toBasicUint64();
                if(v1 >= v2) {
                    return condition->content->execute(o);
                }
            } 
        }

        iterator->next();
    }

    return nullptr;
}


}
