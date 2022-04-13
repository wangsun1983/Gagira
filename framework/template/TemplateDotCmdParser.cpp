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
        printf("DotCmdParser command is %s \n",command->toChars());
        if(command->size() > 1) {
            objItem->fieldNames = command->split(".");
            if(objItem->fieldNames == nullptr) {
                objItem->fieldNames = createArrayList<String>();
                objItem->fieldNames->add(command);
            }
            printf("item fieldNames size is %d \n",objItem->fieldNames->size());
        }
    }

    item = Cast<HtmlTemplateItem>(objItem);
}

HtmlTemplateItem _TemplateDotCmdParser::getTemplateItem() {
    if(item == nullptr) {
        printf("item is nullptr \n");
    } else {
        printf("item is not nullptr \n");
    }
    return item;
}

}
