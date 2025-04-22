#include "TemplateBreakParser.hpp"
#include "TemplateBreakItem.hpp"

using namespace obotcha;

namespace gagira {

_TemplateBreakParser::_TemplateBreakParser() {
    //do nothing
}

TemplateItem _TemplateBreakParser::doParse() {
    return TemplateBreakItem::New();
}

}


