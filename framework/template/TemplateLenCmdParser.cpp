#include "TemplateLenCmdParser.hpp"

using namespace obotcha;

namespace gagira {

_TemplateLenCmdParser::_TemplateLenCmdParser() {

}

void _TemplateLenCmdParser::doParse(String) {
    item = HtmlTemplateLenItem::New();
}

HtmlTemplateItem _TemplateLenCmdParser::getTemplateItem() {
    return item;
}

}

