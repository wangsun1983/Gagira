#include "TemplatePrintParser.hpp"
#include "TemplatePrintItem.hpp"
#include "Log.hpp"

namespace gagira {

_TemplatePrintParser::_TemplatePrintParser(String cmd,bool is_write) {
    //cmd is like ("aaa is #1",b)
    //we should remove left/right brace.
    int left_index = cmd->indexOf("(");
    int right_index = cmd->indexOf(")");
    mCmd = cmd->subString(left_index + 1,right_index - left_index - 1);
    isWrite = is_write;
    mType = Print;
}

TemplateItem _TemplatePrintParser::doParse() {
    TemplatePrintItem printItem = TemplatePrintItem::New(isWrite);

    auto cmd_chars = mCmd->toChars();
    int index = 1;
    for(;index < mCmd->size();index++) {
        if(cmd_chars[index] == '\"') {
            if(cmd_chars[index - 1] =='\\') {
                continue;
            }
            break;
        }
    }

    String expr = mCmd->subString(1,index - 1);
    printItem->setExpress(expr);
    //try to find next params
    for(;index < mCmd->size();index++) {
        if(cmd_chars[index] == ',') {
            break;
        }
    }
    if(index != mCmd->size()) {
        String param_str = mCmd->subString(index + 1,mCmd->size() - index - 1);
        if(param_str->contains(",")) {
            auto params = param_str->split(",");
            printItem->setParams(params);
        } else {
            auto params = ArrayList<String>::New();
            params->add(param_str);
            printItem->setParams(params);
        }
    }
    
    return printItem;
}


}
