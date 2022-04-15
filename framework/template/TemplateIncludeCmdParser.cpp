#include "TemplateIncludeCmdParser.hpp"
#include "FileInputStream.hpp"
#include "Log.hpp"

using namespace obotcha;

namespace gagira {

_TemplateIncludeCmdParser::_TemplateIncludeCmdParser() {

}

void _TemplateIncludeCmdParser::doParse(String cmd) {
    cmd = cmd->trim();
    File includeFile = createFile(cmd);
    if(!includeFile->exists()) {
        LOG(ERROR)<<"File,Path "<<cmd->toChars()<<",not exist";
        return;
    }
    FileInputStream stream = createFileInputStream(createFile(cmd));
    stream->open();
    String content = stream->readAll()->toString();
    item = createHtmlTemplateIncludeItem();
    item->templateItem = createHtmlTemplate();
    item->templateItem->import(content);
    stream->close();
}

HtmlTemplateItem _TemplateIncludeCmdParser::getTemplateItem() {
    return item;
}



}

