#ifndef __GAGRIA_TEMPLATE_PARSER_HELPER_HPP__
#define __GAGRIA_TEMPLATE_PARSER_HELPER_HPP__

#include "ArrayList.hpp"
#include "String.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateParserHelper) {
public:
    static ArrayList<String>ParseStatement(String cmd,char splitTag);
};

}

#endif
