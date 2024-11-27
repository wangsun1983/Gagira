#include "TemplatePrintItem.hpp"
#include "TemplateFunctionParser.hpp"
#include "Log.hpp"

namespace gagira {

_TemplatePrintItem::_TemplatePrintItem(bool is_write) {
    isWrite = is_write;
}

void _TemplatePrintItem::setExpress(String expr) {
    mExpress = expr;
}

String _TemplatePrintItem::getExpress() {
    return mExpress;
}

void _TemplatePrintItem::setParams(ArrayList<String> params) {
    this->mParams = params;
}

ArrayList<String> _TemplatePrintItem::getParams() {
    return mParams;
}

/**
 * let int a = 10;
 * let string b = "hello"
 * print("example #1,#1,#2",a,b);
 * =>result:10,10,hello
 */
TemplateScopedValue _TemplatePrintItem::execute(TemplateScopedValueContainer scopedvalueContainer,TemplateObjectContainer objContainer) {
    //auto scopedValues = scopedvalueContainer->getCurrent();
    auto obj = objContainer->getCurrent();
    printf("TemplatePrintItem execute start \n");
    //search #
    String result = String::New("");
    auto *expr_chars = mExpress->toChars();

    int str_start = 0;
    int idx_start = 0;
    for(int index = 0;index < mExpress->size();index++) {
        if(expr_chars[index] == '#') {
            idx_start = index;
            while(index < mExpress->size() - 1) {
                if(expr_chars[index + 1] < '9' && expr_chars[index + 1] > '0') {
                    index++;
                    continue;
                }
                break;
            }

            if(idx_start != index) {
                auto str = mExpress->subString(str_start,idx_start - str_start);
                result = result->append(str);
                printf("printitem,trace1 result is %s\n",result->toChars());
                auto idx = mExpress->subString(idx_start + 1,index - idx_start);
                auto idx_int = idx->toBasicInt();
                //#[x],x is from 1,so we need minus 1 as index
                String param = mParams->get(idx_int - 1);
                printf("param is %s \n",param->toChars());
                TemplateFunctionParser func_parser = TemplateFunctionParser::New(param);
                auto func_item = func_parser->doParse();
                auto r1 = result->append(func_item->execute(scopedvalueContainer,objContainer)->toString());
                result = result->append(r1);
                printf("printitem,trace2 rresult is %s\n",result->toChars());
                str_start = index + 1;
            }
        }
    }

    if(!isWrite) {
        LOG(INFO)<<result->toChars();
        return nullptr;
    } else {
        return TemplateScopedValue::New(result);
    }
}

}

