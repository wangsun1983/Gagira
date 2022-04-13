#ifndef __GAGRIA_HTML_TEMPLATE_FUNCTION_HPP__
#define __GAGRIA_HTML_TEMPLATE_FUNCTION_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

namespace gagira {

using HtmlTemplateFunction = std::function<String(String)>;

}

#endif
