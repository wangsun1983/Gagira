#include "TemplateFunctionCmdParser.hpp"
#include "HtmlTemplateObjectItem.hpp"
#include "HtmlTemplateTextItem.hpp"

using namespace obotcha;

namespace gagira {

_TemplateFunctionCmdParser::_TemplateFunctionCmdParser(String name,HtmlTemplateFunction func) {
    mName = name;
    mFunc = func;
}

void _TemplateFunctionCmdParser::doParse(String cmd) {
    cmd = cmd->trim();
    mItem = createHtmlTemplateFunctionItem(mName,mFunc);
    mItem->mFunc = mFunc;

    ArrayList<String> params = cmd->split(" ");
    auto iterator = params->getIterator();
    while(iterator->hasValue()) {
        String param = iterator->getValue();
        if(param != nullptr && param->size() != 0) {
            if(param->startsWith(".")) {
                TemplateDotCmdParser parser = createTemplateDotCmdParser();
                parser->doParse(param);
                HtmlTemplateItem item = parser->getTemplateItem();
                if(item != nullptr) {
                    mItem->mItems->add(item);
                }
            } else {
                HtmlTemplateItem item = createHtmlTemplateTextItem(param);
                mItem->mItems->add(item);
            }
        }

        iterator->next();
    }
}

HtmlTemplateItem _TemplateFunctionCmdParser::getTemplateItem() {
    return mItem;
}


}
