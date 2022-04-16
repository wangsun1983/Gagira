#include "TemplateLenCmdParser.hpp"

using namespace obotcha;

namespace gagira {

_TemplateLenCmdParser::_TemplateLenCmdParser() {

}

void _TemplateLenCmdParser::doParse(String) {
    item = createHtmlTemplateLenItem();
}

HtmlTemplateItem _TemplateLenCmdParser::getTemplateItem() {
    return item;
}

}

