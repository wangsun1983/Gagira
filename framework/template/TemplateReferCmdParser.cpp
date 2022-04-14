#include "TemplateReferCmdParser.hpp"

using namespace obotcha;

namespace gagira {

_TemplateReferCmdParser::_TemplateReferCmdParser() {

}

void _TemplateReferCmdParser::doParse(String cmd) {
    item = createHtmlTemplateReferItem(cmd);
}

HtmlTemplateItem _TemplateReferCmdParser::getTemplateItem() {
    return item;
}



}
