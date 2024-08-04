#include "HtmlTemplateRangeItem.hpp"
#include "OStdInstanceOf.hpp"
#include "StringBuffer.hpp"
#include "HtmlTemplateReferItem.hpp"

using namespace obotcha;

namespace gagira {

_HtmlTemplateRangeItem::_HtmlTemplateRangeItem() {
    mIndexName = nullptr;
    mNoValueTemplate = nullptr;
    mTemplate = nullptr;
}

String _HtmlTemplateRangeItem::toString(Object o) {
    int size = o->__getContainerSize("");
    
    if(size == 0) {
        if(mNoValueTemplate != nullptr) {
            return mNoValueTemplate->execute(o);
        }

        return nullptr;
    }

    StringBuffer buffer = StringBuffer::New();

    for(int i = 0;i < size;i++) {
        Object obj = o->__getListItemObject("",i);
        if(mIndexName != nullptr) {
            //for display ,cout from 1 instead of 0
            st(HtmlTemplateReferItem)::setValue(mIndexName,String::New(i + 1));
            buffer->append(mTemplate->execute(obj));
        }
    }

    return buffer->toString();
}

}
