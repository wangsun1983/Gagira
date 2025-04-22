#ifndef __GAGRIA_TEMPLATE_RETURN_PARSER_HPP__
#define __GAGRIA_TEMPLATE_RETURN_PARSER_HPP__

#include "TemplateParser.hpp"
#include "TemplateInstruction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateReturnParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateReturnParser(String cmd);
    TemplateItem doParse();

private:
    String mCmd;
};

}

#endif
