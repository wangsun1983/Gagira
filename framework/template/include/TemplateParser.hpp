#ifndef __GAGRIA_TEMPLATE_PARSER_HPP__
#define __GAGRIA_TEMPLATE_PARSER_HPP__

#include "TemplateItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateParser) {
public:
    virtual void inject(String){};
    virtual TemplateItem doParse() = 0;
};

}

#endif
