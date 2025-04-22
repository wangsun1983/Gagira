#ifndef __GAGRIA_TEMPLATE_SCOPED_VALUE_HELPER_HPP__
#define __GAGRIA_TEMPLATE_SCOPED_VALUE_HELPER_HPP__

#include "TemplateScopedValue.hpp"
#include "TemplateScopedValueContainer.hpp"
#include "TemplateObjectContainer.hpp"
#include "OStdReturnValue.hpp"
#include "ReflectMethod.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateScopedValueHelper) {
public:
    static TemplateScopedValue Get(String tag,TemplateScopedValueContainer scopdevalueContainer,TemplateObjectContainer objContainer);
    static void Set(String tag,String value,TemplateScopedValueContainer,TemplateObjectContainer);
    static MethodField GetMethod(String tag,TemplateObjectContainer objContainer);
    
private:
    enum Type {
        Scoped = 0,
        Obj,
    };

    static TemplateScopedValue SeekInObject(String tag,Object obj);
    static void UpdateInObject(String tag,String value,Object obj);
    static DefRet(String,String,int) disassembleParam(String);
};

}

#endif
