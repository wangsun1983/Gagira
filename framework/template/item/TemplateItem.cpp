#include "TemplateItem.hpp"

namespace gagira {

_TemplateItem::_TemplateItem() {
    mNextItems = ArrayList<sp<_TemplateItem>>::New();
    mScopeValues = HashMap<String,TemplateScopedValue>::New();
}

}

