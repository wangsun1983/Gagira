#include "TemplateIncludeCmdParser.hpp"
#include "FileInputStream.hpp"
#include "Log.hpp"

using namespace obotcha;

namespace gagira {

_TemplateIncludeCmdParser::_TemplateIncludeCmdParser() {

}

void _TemplateIncludeCmdParser::doParse(String cmd) {
    cmd = cmd->trim();
    File includeFile = File::New(cmd);
    if(!includeFile->exists()) {
        LOG(ERROR)<<"File,Path "<<cmd->toChars()<<",not exist";
        return;
    }
    FileInputStream stream = FileInputStream::New(File::New(cmd));
    stream->open();
    String content = stream->readAll()->toString();
    item = HtmlTemplateIncludeItem::New();
    item->templateItem = HtmlTemplate::New();
    item->templateItem->import(content);
    stream->close();
}

HtmlTemplateItem _TemplateIncludeCmdParser::getTemplateItem() {
    return item;
}



}

