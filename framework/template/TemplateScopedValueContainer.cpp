#include "ForEveryOne.hpp"
#include "TemplateScopedValueContainer.hpp"

using namespace obotcha;

namespace gagira {

_TemplateScopedValueContainer::_TemplateScopedValueContainer(HashMap<String,TemplateScopedValue> value) {
    mScopedValues = LinkedList<HashMap<String,TemplateScopedValue>>::New();
    mScopedValues->putLast(value);
}

HashMap<String,TemplateScopedValue> _TemplateScopedValueContainer::getCurrent() {
    return mScopedValues->peekLast();
}

void _TemplateScopedValueContainer::createNew() {
    mScopedValues->putLast(HashMap<String,TemplateScopedValue>::New());
}

void _TemplateScopedValueContainer::removeCurrent() {
    mScopedValues->takeLast();
}

void _TemplateScopedValueContainer::addScopedValue(String name,TemplateScopedValue scoped_value) {
    auto current_map = mScopedValues->peekLast();
    current_map->put(name,scoped_value);
}

TemplateScopedValue _TemplateScopedValueContainer::getScopedValue(String name) {
    //search from the last
    int size = mScopedValues->size();
    for(int index = size - 1;index >= 0;index--) {
        auto scoped_values = mScopedValues->at(index);
        auto scoped_value = scoped_values->get(name);
        if(scoped_value != nullptr) {
            return scoped_value;
        }
    }

    return nullptr;
}

}
