#include "HtmlTemplateRangeItem.hpp"
#include "OStdInstanceOf.hpp"
#include "StringBuffer.hpp"
#include "HtmlTemplateReferItem.hpp"

using namespace obotcha;

namespace gagira {

_HtmlTemplateRangeItem::_HtmlTemplateRangeItem() {
    mIndexName = nullptr;
}

String _HtmlTemplateRangeItem::toString(Object o) {
    int size = o->__getContainerSize("");
    StringBuffer buffer = createStringBuffer();

    for(int i = 0;i < size;i++) {
        Object obj = o->__getListItemObject("",i);
        if(mIndexName != nullptr) {
            //for display ,cout from 1 instead of 0
            st(HtmlTemplateReferItem)::setValue(mIndexName,createString(i + 1));
            buffer->append(mTemplate->execute(o));
        }
    }

    return buffer->toString();
}

}
