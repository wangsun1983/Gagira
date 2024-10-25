#ifndef __GAGRIA_TEMPLATE_COMMENT_ITEM_HPP__
#define __GAGRIA_TEMPLATE_COMMENT_ITEM_HPP__

#include "TemplateItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateCommentItem) IMPLEMENTS(TemplateItem){
public:
    _TemplateCommentItem();
    void addComment(String);
    String toText();
    void dump();

    TemplateScopedValue execute(HashMap<String,TemplateScopedValue> scopedValues,Object obj);
private:
    ArrayList<String> mComments;
};

}

#endif
