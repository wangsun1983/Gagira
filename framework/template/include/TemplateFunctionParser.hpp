#ifndef __GAGRIA_TEMPLATE_FUNCTION_PARSER_HPP__
#define __GAGRIA_TEMPLATE_FUNCTION_PARSER_HPP__

#include "HashMap.hpp"

#include "TemplateParser.hpp"
#include "TemplateInstruction.hpp"
#include "TemplateFunctionItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateFunctionParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateFunctionParser(String cmd);
    TemplateItem doParse();

private:
    static HashMap<String,Integer> PriorityMaps;
    String mCmd;

    static char OperatorCharArray[128];
    bool isOperator(char);
    bool isUnaryOperator(const char*,int index);
    
    String tranUnaryOperator(const char*,int index);

    TemplateFuncExpression createVariableExpression(String);

};

}

#endif
