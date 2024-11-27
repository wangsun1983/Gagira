#ifndef __GAGRIA_TEMPLATE_SCOPED_VALUE_SEEKER_HPP__
#define __GAGRIA_TEMPLATE_SCOPED_VALUE_SEEKER_HPP__

#include "TemplateScopedValue.hpp"
#include "TemplateScopedValueContainer.hpp"
#include "TemplateObjectContainer.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateScopedValueSeeker) {
public:
    static TemplateScopedValue Get(String tag,TemplateScopedValueContainer scopdevalueContainer,TemplateObjectContainer objContainer);
    static TemplateScopedValue SeekInObject(String tag,Object obj);

private:
    enum Type {
        Scoped = 0,
        Obj,
    };
};

}

#endif
