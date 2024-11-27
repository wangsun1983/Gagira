#ifndef __GAGRIA_TEMPLATE_HPP__
#define __GAGRIA_TEMPLATE_HPP__

#include "String.hpp"
#include "ArrayList.hpp"
#include "TemplateItem.hpp"
#include "TemplateInstruction.hpp"
#include "TemplateScopedValueContainer.hpp"
#include "TemplateObjectContainer.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(Template) IMPLEMENTS(TemplateInstruction) {
public:
    _Template(String text);

    String execute(HashMap<String,TemplateScopedValue> scopedValues,Object obj);
private:
    enum Status {
        ParseHtml = 0,
        ParseCommand
    };
    
    String mText;
    int mStatus;
    ArrayList<TemplateItem> mItems;

    TemplateScopedValueContainer mScopedValueContainer;
    TemplateObjectContainer mObjectContainer;
    
    void startAnalyse();
};

}

#endif
