#ifndef __GAGRIA_HTML_TEMPLATE_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_ITEM_HPP__

#include "String.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(HtmlTemplateItem) {

public:
    virtual String toString(Object o) = 0;
    virtual Object toObject(Object) {return nullptr;}

};

}

#endif
