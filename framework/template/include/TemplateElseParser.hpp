#ifndef __GAGRIA_TEMPLATE_ELSE_PARSER_HPP__
#define __GAGRIA_TEMPLATE_ELSE_PARSER_HPP__


#include "TemplateParser.hpp"
#include "TemplateInstruction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateElseParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateElseParser();
    TemplateItem doParse();
};

}

#endif
