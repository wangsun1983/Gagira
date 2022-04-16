#include "HtmlTemplateWithItem.hpp"
#include "HtmlTemplateIndexItem.hpp"
#include "OStdInstanceOf.hpp"

using namespace obotcha;

namespace gagira {

_HtmlTemplateWithItem::_HtmlTemplateWithItem() {

}

String _HtmlTemplateWithItem::toString(Object o) {
    Object obj = nullptr;
    if(pipline != nullptr) {
        if(IsInstance(HtmlTemplateIndexItem,pipline)) {
            obj = pipline->toObject(o);
        }
    }
    if(content != nullptr && obj != nullptr) {
        return content->execute(obj);
    }

    return nullptr;
}



}

