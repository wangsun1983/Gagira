#include "HtmlTemplateTextItem.hpp"

using namespace obotcha;

namespace gagira {

_HtmlTemplateTextItem::_HtmlTemplateTextItem(String v) {
    text = v;
}

String _HtmlTemplateTextItem::toString(Object o) {
    return text;
}

}
