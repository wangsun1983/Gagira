#include "TemplateReturnParser.hpp"
#include "TemplateFunctionParser.hpp"
#include "TemplateReturnItem.hpp"

using namespace obotcha;

namespace gagira {

_TemplateReturnParser::_TemplateReturnParser(String cmd) {
    mCmd = cmd;
}

TemplateItem _TemplateReturnParser::doParse() {
    if(mCmd != nullptr && !mCmd->trimAll()->isEmpty()) {
        auto parser = TemplateFunctionParser::New(mCmd);
        auto result = TemplateReturnItem::New();
        result->setFunctionItem(parser->doParse());
        return result;   
    } else {
        return TemplateReturnItem::New();
    }
    
}


}

