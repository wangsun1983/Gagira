#ifndef __GAGRIA_TEMPLATE_PARSER_DISPATCHER_HPP__
#define __GAGRIA_TEMPLATE_PARSER_DISPATCHER_HPP__

#include "String.hpp"
#include "TemplateParser.hpp"
#include "TemplateInstruction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateParserDispatcher) IMPLEMENTS(TemplateInstruction) {
public:
    TemplateParser apply(String text);
};

}

#endif
