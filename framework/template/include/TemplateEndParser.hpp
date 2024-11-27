#ifndef __GAGRIA_TEMPLATE_END_PARSER_HPP__
#define __GAGRIA_TEMPLATE_END_PARSER_HPP__

#include "TemplateParser.hpp"
#include "TemplateInstruction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateEndParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateEndParser();
    TemplateItem doParse();

private:
    String mCmd;
};

}

#endif
