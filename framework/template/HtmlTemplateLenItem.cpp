#include "HtmlTemplateLenItem.hpp"
#include "OStdInstanceOf.hpp"

using namespace obotcha;

class _TemplateLenCmdParser;

namespace gagira {

_HtmlTemplateLenItem::_HtmlTemplateLenItem() {

}

String _HtmlTemplateLenItem::toString(Object o) {
    int size = o->__getContainerSize("");
    return createString(size);
}

}