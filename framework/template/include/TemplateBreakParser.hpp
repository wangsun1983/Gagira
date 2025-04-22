#ifndef __GAGRIA_TEMPLATE_BREAK_PARSER_HPP__
#define __GAGRIA_TEMPLATE_BREAK_PARSER_HPP__

#include "TemplateParser.hpp"
#include "TemplateInstruction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateBreakParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateBreakParser();
    TemplateItem doParse();
};

}

#endif
