#include "TemplateItem.hpp"

namespace gagira {

_TemplateItem::_TemplateItem() {
    mNextItems = ArrayList<sp<_TemplateItem>>::New();
    mScopeValues = HashMap<String,TemplateScopedValue>::New();
}

// bool _TemplateItem::isDirectReturn() {
//     return false;
// }

#ifdef DEBUG_TEMPLATE_ITEM_COMMAND    
void _TemplateItem::setCmd(String c) {
    mCmd = c;
}

String _TemplateItem::getCmd() {
    return mCmd;
}
#endif

}

