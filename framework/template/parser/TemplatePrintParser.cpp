#include "TemplatePrintParser.hpp"
#include "TemplatePrintItem.hpp"
#include "Log.hpp"

namespace gagira {

_TemplatePrintParser::_TemplatePrintParser(String cmd,bool is_write) {
    //cmd is like ("aaa is #1",b)
    //we should remove left/right brace.
    printf("templateprintparser construct \n");
    int left_index = cmd->indexOf("(");
    int right_index = cmd->indexOf(")");
    mCmd = cmd->subString(left_index + 1,right_index - left_index - 1);
    isWrite = is_write;
    printf("templateprintparser construct end \n");
}

TemplateItem _TemplatePrintParser::doParse() {
    printf("printparser do parse \n");
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
    printf("printparser do parse trace1\n");
    //try to find next params
    for(;index < mCmd->size();index++) {
        if(cmd_chars[index] == ',') {
            break;
        }
    }
    printf("index is %d,cmd size is %d \n",index,mCmd->size());
    if(index != mCmd->size()) {
        String param_str = mCmd->subString(index + 1,mCmd->size() - index - 1);
        printf("param_str is %s \n",param_str->toChars());
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
