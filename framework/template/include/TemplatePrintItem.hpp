#ifndef __GAGRIA_TEMPLATE_PRINT_ITEM_HPP__
#define __GAGRIA_TEMPLATE_PRINT_ITEM_HPP__

#include "String.hpp"
#include "TemplateItem.hpp"
#include "Template.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplatePrintItem) IMPLEMENTS(TemplateItem) {

public:
    _TemplatePrintItem(bool is_write = false);
    void setExpress(String);
    String getExpress();

    void setParams(ArrayList<String>);
    ArrayList<String> getParams();

    TemplateScopedValue execute(TemplateScopedValueContainer,TemplateObjectContainer);
private:
    bool isWrite;
    String mExpress;
    ArrayList<String> mParams;
    
};

}

#endif
