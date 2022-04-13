#ifndef __GAGRIA_HTML_TEMPLATE_CONDITION_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_CONDITION_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "HtmlTemplateObjectItem.hpp"

using namespace obotcha;

namespace gagira {

class _TemplateConditionCmdParser;

DECLARE_CLASS(HtmlTemplateCondition) {
public:
    _HtmlTemplateCondition();
    String condition;
    HtmlTemplateObjectItem uniqueItem; //{{if .value}}
    HtmlTemplateObjectItem objItem1;
    HtmlTemplateObjectItem objItem2;
    String stringItem1;
    String stringItem2;

    //TODO,this contents may also contains {{.}}
    String content;
    bool isFinalCondition;
};

DECLARE_CLASS(HtmlTemplateConditionItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    friend class _TemplateConditionCmdParser;
    _HtmlTemplateConditionItem();
    String toString(Object o);

private:
    ArrayList<HtmlTemplateCondition> conditions;
};

}

#endif
