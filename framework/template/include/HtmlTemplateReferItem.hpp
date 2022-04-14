#ifndef __GAGRIA_HTML_TEMPLATE_REFER_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_REFER_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "HtmlTemplateObjectItem.hpp"
#include "ArrayList.hpp"
#include "ThreadLocal.hpp"
#include "HashMap.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HtmlTemplateReferItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    _HtmlTemplateReferItem(String);
    String toString(Object o);
    static void setValue(String,String);
    String getName();

private:
    static ThreadLocal<HashMap<String,String>> values;
    
    String paramName;
};

}

#endif
