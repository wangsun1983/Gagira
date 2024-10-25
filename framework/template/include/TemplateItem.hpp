#ifndef __GAGRIA_TEMPLATE_ITEM_HPP__
#define __GAGRIA_TEMPLATE_ITEM_HPP__

#include "String.hpp"

#include "ArrayList.hpp"
#include "HashMap.hpp"
#include "TemplateScopedValue.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateItem) {
public:
    enum Type {
        Text = 0,
    };

    _TemplateItem();
    
    virtual TemplateScopedValue execute(HashMap<String,TemplateScopedValue> scopedValues,Object obj) = 0;
    virtual void dump() {};

    void setType(int type);
    int getType();

protected:
    ArrayList<sp<_TemplateItem>> mNextItems;
    HashMap<String,TemplateScopedValue> mScopeValues;
    int mType;
};

}

#endif
