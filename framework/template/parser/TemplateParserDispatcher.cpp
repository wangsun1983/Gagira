#include "TemplateParserDispatcher.hpp"
#include "TemplateCommentParser.hpp"
#include "TemplateLetParser.hpp"
#include "TemplatePrintParser.hpp"
#include "TemplateConditionParser.hpp"
#include "TemplateElseParser.hpp"
#include "TemplateFunctionParser.hpp"
#include "TemplateInParser.hpp"
#include "TemplateLoopParser.hpp"
#include "TemplateEndParser.hpp"

namespace gagira {

TemplateParser _TemplateParserDispatcher::Apply(String text) {
    text = text->trim();
    printf("apply text is %s \n",text->toChars());
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
    } else if (text->startsWith(IfCommand)) {
        /**
         * do not trim if
         * we should parse like the following
         * 
         * if(xxxx)
         *    xxxx
         *    if(yyyy)
         *      yyyy
         *    fi
         * elif(zzzz)
         *    zzzz
         * fi
         * 
         *  */
        return TemplateConditionParser::New(text->subString(IfCommand->size(),text->size() - IfCommand->size())); 
    } else if(text->startsWith(ElseIfCommand)) {
        auto parser = TemplateConditionParser::New(text->subString(ElseIfCommand->size(),text->size() - ElseIfCommand->size()));
        parser->setType(st(TemplateParser)::Elif);
        return parser;
    } else if (text->startsWith(ElseCommand)) {
        return TemplateElseParser::New(); 
    } else if (text->startsWith(EndCommand)) {
        return TemplateEndParser::New(); 
    } else if (text->startsWith(InCommand)) {
        return TemplateInParser::New(text->subString(InCommand->size(),text->size() - InCommand->size()));
    } else if (text->startsWith(LoopCommand)) {
        return TemplateLoopParser::New(text->subString(LoopCommand->size(),text->size() - LoopCommand->size()));
    }

    return TemplateFunctionParser::New(text);
}

}
