#ifndef __GAGRIA_HTML_TEMPLATE_OBJECT_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_OBJECT_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

class _TemplateDotCmdParser;

namespace gagira {

DECLARE_CLASS(HtmlTemplateObjectItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    friend class _TemplateDotCmdParser;

    _HtmlTemplateObjectItem();
    String toString(Object o);

private:
    String _objectToString(Object o);
    ArrayList<String> fieldNames;

};

}

#endif
