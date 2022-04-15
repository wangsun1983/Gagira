#include "TemplateRangeCmdParser.hpp"
#include "HtmlTemplateRangeItem.hpp"

using namespace obotcha;

namespace gagira {

_TemplateRangeCmdParser::_TemplateRangeCmdParser() {
    item = nullptr;
}

/*
{{range $index, .}}
    <tr>
        <td>{{$index}}</td>
        <td>{{.Name}}</td>
        <td>{{.Age}}</td>
        <td>{{.Male}}</td>
    </tr>
{{end}}
*/
void _TemplateRangeCmdParser::doParse(String cmd) {
    item = createHtmlTemplateRangeItem();
    int index = cmd->indexOf(",");
    if(index > 0) {
        String param = cmd->subString(0,index)->trim();
        if(param->startsWith("$")) {
            item->mIndexName = param->subString(1,param->size() - 1);
        }
    }
}

HtmlTemplateItem _TemplateRangeCmdParser::getTemplateItem() {
    return item;
}



}
