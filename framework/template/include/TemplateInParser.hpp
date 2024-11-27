#ifndef __GAGRIA_TEMPLATE_IN_PARSER_HPP__
#define __GAGRIA_TEMPLATE_IN_PARSER_HPP__

#include "TemplateParser.hpp"
#include "TemplateInstruction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateInParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateInParser(String cmd);
    TemplateItem doParse();

private:
    String mCmd;
};

}

#endif
