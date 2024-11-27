#include "TemplateCommentParser.hpp"
#include "TemplateCommentItem.hpp"

namespace gagira {

_TemplateCommentParser::_TemplateCommentParser(String cmd) {
    mCmd = cmd->trim();
    mType = Comment;
}

TemplateItem _TemplateCommentParser::doParse() {
    if(mCmd == nullptr) {
        return nullptr;
    }

    if(mItem == nullptr) {
        mItem = TemplateCommentItem::New();
    }

    if(mCmd->endsWith(EndComment)) {
        if(mCmd->size() != EndComment->size()) {
            mItem->addComment(mCmd->subString(0,mCmd->size() - EndComment->size()));
        }
        mCmd = nullptr;

        return mItem;
    } else {
        mItem->addComment(mCmd);
        mCmd = nullptr;
    }

    return nullptr; 
}

void _TemplateCommentParser::inject(String comment) {
    mCmd = comment;
}

}