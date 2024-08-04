#include "TemplateDefineCmdParser.hpp"

using namespace obotcha;

namespace gagira {

_TemplateDefineCmdParser::_TemplateDefineCmdParser() {

}

void _TemplateDefineCmdParser::doParse(String cmd) {
    cmd = cmd->trim();
    item = HtmlTemplateDefineItem::New();
    
    if(cmd->startsWith("\"") && cmd->endsWith("\"")) {
        item->mName = cmd->subString(1,cmd->size() - 2);
    }
}

HtmlTemplateItem _TemplateDefineCmdParser::getTemplateItem() {
    return item;
}

}
