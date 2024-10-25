#include "TemplateParserDispatcher.hpp"
#include "TemplateCommentParser.hpp"
#include "TemplateLetParser.hpp"
#include "TemplatePrintParser.hpp"

namespace gagira {

TemplateParser _TemplateParserDispatcher::apply(String text) {
    text = text->trim();
    if(text->startsWith(LineComment)) {
        return nullptr;
    } else if(text->startsWith(StartComment)) {
        return TemplateCommentParser::New(text->subString(StartComment->size(),text->size() - StartComment->size()));
    } else if (text->startsWith(LetCommand)) {
        return TemplateLetParser::New(text->subString(LetCommand->size(),text->size() - LetCommand->size()));
    } else if (text->startsWith(PrintCommand)) {
        return TemplatePrintParser::New(text->subString(PrintCommand->size(),text->size() - PrintCommand->size()));
    } else if (text->startsWith(WriteCommand)) {
        return TemplatePrintParser::New(text->subString(PrintCommand->size(),text->size() - PrintCommand->size()),true);
    }

    return nullptr;
}

}
