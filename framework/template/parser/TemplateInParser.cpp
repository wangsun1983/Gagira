#include "TemplateInParser.hpp"
#include "TemplateInItem.hpp"

using namespace obotcha;

namespace gagira {

_TemplateInParser::_TemplateInParser(String cmd) {
    mType = In;
    mCmd = cmd->trim();
}

TemplateItem _TemplateInParser::doParse() {
    TemplateInItem item = TemplateInItem::New();
    if(mCmd->startsWith(".")) {
        item->setInName(mCmd->subString(1,mCmd->size() - 1));
    }
    return item;
}

}