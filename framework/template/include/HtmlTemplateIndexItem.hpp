#ifndef __GAGRIA_HTML_TEMPLATE_INDEX_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_INDEX_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

class _TemplateIndexCmdParser;

namespace gagira {

DECLARE_CLASS(HtmlTemplateIndexItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    friend class _TemplateIndexCmdParser;

    _HtmlTemplateIndexItem();
    String toString(Object o);
    Object toObject(Object o);

private:
    int index;

};

}

#endif
