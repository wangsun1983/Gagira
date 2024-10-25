#ifndef __GAGRIA_TEMPLATE_COMMENT_PARSER_HPP__
#define __GAGRIA_TEMPLATE_COMMENT_PARSER_HPP__

#include "TemplateParser.hpp"
#include "TemplateCommentItem.hpp"
#include "TemplateInstruction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateCommentParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateCommentParser(String cmd);
    TemplateItem doParse();
    void inject(String);
    
private:
    String mCmd;
    TemplateCommentItem mItem;
    bool isComplete;
};

}

#endif
