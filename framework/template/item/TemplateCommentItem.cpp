#include "TemplateCommentItem.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

_TemplateCommentItem::_TemplateCommentItem() {
    mComments = ArrayList<String>::New();
}

void _TemplateCommentItem::addComment(String cmd) {
    mComments->add(cmd);
}

void _TemplateCommentItem::dump() {
    printf("-------------------- \n");
    printf("Template Comment Item: \n");
    ForEveryOne(comment,mComments) {
        printf("%s \n",comment->toChars());
    }
    printf("-------------------- \n");
}

TemplateScopedValue _TemplateCommentItem::execute(HashMap<String,TemplateScopedValue> scopedValues,Object obj) {
    return nullptr;
}

}


