#include "TemplateWithCmdParser.hpp"
#include "TemplateIndexCmdParser.hpp"
#include "HtmlTemplate.hpp"

using namespace obotcha;

namespace gagira {

_TemplateWithCmdParser::_TemplateWithCmdParser() {

}

void _TemplateWithCmdParser::doParse(String cmd) {
    item = createHtmlTemplateWithItem();

    cmd = cmd->trim();
    if(cmd->startsWithIgnoreCase(st(HtmlTemplate)::IndexCommand)) {
        cmd = cmd->subString(st(HtmlTemplate)::IndexCommand->size(),cmd->size() 
                - st(HtmlTemplate)::IndexCommand->size());
        TemplateIndexCmdParser parser = createTemplateIndexCmdParser();
        parser->doParse(cmd);
        item->pipline = parser->getTemplateItem();
    }
}

HtmlTemplateItem _TemplateWithCmdParser::getTemplateItem() {
    return item;
}

}

