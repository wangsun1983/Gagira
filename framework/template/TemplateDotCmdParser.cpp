#include "TemplateDotCmdParser.hpp"
#include "HtmlTemplateObjectItem.hpp"

using namespace obotcha;

namespace gagira {

_TemplateDotCmdParser::_TemplateDotCmdParser() {

}

void _TemplateDotCmdParser::doParse(String command) {
    HtmlTemplateObjectItem objItem = createHtmlTemplateObjectItem();
    if(command != nullptr) {
        command = command->trimAll();
        if(command->size() > 1) {
            objItem->fieldNames = command->split(".");
            if(objItem->fieldNames == nullptr) {
                objItem->fieldNames = createArrayList<String>();
                objItem->fieldNames->add(command);
            }
        }
    }

    item = Cast<HtmlTemplateItem>(objItem);
}

HtmlTemplateItem _TemplateDotCmdParser::getTemplateItem() {
    return item;
}

}
