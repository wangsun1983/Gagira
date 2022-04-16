#include "TemplateIndexCmdParser.hpp"

using namespace obotcha;

namespace gagira {

_TemplateIndexCmdParser::_TemplateIndexCmdParser() {

}

void _TemplateIndexCmdParser::doParse(String cmd) {
    item = createHtmlTemplateIndexItem();
    //find last index
    int size = cmd->size();
    int start = size - 1;
    int end = 0;
    const char *p = cmd->toChars();

    for(;start > 0;start--) {
        if(isDigit(p[start])) {
            if(end == 0) {
                end = start;
            }
            continue;
        }
        break;
    }
    String index_str = cmd->subString(start + 1,end - start);
    item->index = index_str->toBasicInt();
}

bool _TemplateIndexCmdParser::isDigit(unsigned char c) {
    return c <= 0x39 && c >= 0x30;
}

HtmlTemplateItem _TemplateIndexCmdParser::getTemplateItem() {
    return item;
}


}
