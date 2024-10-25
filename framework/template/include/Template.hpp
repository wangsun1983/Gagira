#ifndef __GAGRIA_TEMPLATE_HPP__
#define __GAGRIA_TEMPLATE_HPP__

#include "String.hpp"
#include "ArrayList.hpp"
#include "TemplateItem.hpp"
#include "TemplateInstruction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(Template) IMPLEMENTS(TemplateInstruction) {
public:
    _Template(String text);
private:
    String mText;
    ArrayList<TemplateItem> mItems;

    void startAnalyse();
};

}

#endif
