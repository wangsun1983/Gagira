#ifndef __GAGRIA_TEMPLATE_PRINT_PARSER_HPP__
#define __GAGRIA_TEMPLATE_PRINT_PARSER_HPP__

#include "HashMap.hpp"

#include "TemplateParser.hpp"
#include "TemplateInstruction.hpp"
#include "TemplateFunctionItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplatePrintParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplatePrintParser(String cmd,bool isWrite = false);
    TemplateItem doParse();

private:
    String mCmd;
    bool isWrite;
};

}

#endif
