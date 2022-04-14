#ifndef __GAGRIA_HTML_TEMPLATE_CONDITION_ITEM_HPP__
#define __GAGRIA_HTML_TEMPLATE_CONDITION_ITEM_HPP__

#include "String.hpp"
#include "HtmlTemplateItem.hpp"
#include "ArrayList.hpp"

using namespace obotcha;

namespace gagira {

class _HtmlTemplateObjectItem;
class _TemplateConditionCmdParser;
class _HtmlTemplate;

DECLARE_CLASS(HtmlTemplateCondition) {
public:
    _HtmlTemplateCondition();
    ~_HtmlTemplateCondition();
    String condition;
    sp<_HtmlTemplateObjectItem> uniqueItem; //{{if .value}}
    sp<_HtmlTemplateObjectItem> objItem1;
    sp<_HtmlTemplateObjectItem> objItem2;
    String stringItem1;
    String stringItem2;

    //TODO,this contents may also contains {{.}}
    sp<_HtmlTemplate> content;
    bool isFinalCondition;
};

DECLARE_CLASS(HtmlTemplateConditionItem) IMPLEMENTS(HtmlTemplateItem) {
public:
    friend class _TemplateConditionCmdParser;
    friend class _HtmlTemplate;
    _HtmlTemplateConditionItem();
    String toString(Object o);

private:
    ArrayList<HtmlTemplateCondition> conditions;
};

}

#endif
