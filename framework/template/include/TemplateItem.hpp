#ifndef __GAGRIA_TEMPLATE_ITEM_HPP__
#define __GAGRIA_TEMPLATE_ITEM_HPP__

#include "String.hpp"

#include "ArrayList.hpp"
#include "HashMap.hpp"
#include "TemplateScopedValue.hpp"
#include "TemplateScopedValueContainer.hpp"
#include "TemplateObjectContainer.hpp"
#include "TemplateDebug.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateItem) {
public:
    enum Type {
        Text = 0,
    };

    _TemplateItem();
    
    virtual TemplateScopedValue execute(TemplateScopedValueContainer,TemplateObjectContainer) = 0;
    virtual void dump() {};

    //virtual bool isDirectReturn();
    void setType(int type);
    int getType();

#ifdef DEBUG_TEMPLATE_ITEM_COMMAND
    void setCmd(String);
    String getCmd();
#endif    

protected:
    ArrayList<sp<_TemplateItem>> mNextItems;
    HashMap<String,TemplateScopedValue> mScopeValues;
    int mType;

#ifdef DEBUG_TEMPLATE_ITEM_COMMAND
    String mCmd;
#endif

};

}

#endif
