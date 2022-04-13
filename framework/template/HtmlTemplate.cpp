#include "HtmlTemplate.hpp"
#include "HtmlTemplateTextItem.hpp"
#include "TemplateDotCmdParser.hpp"
#include "StringBuffer.hpp"
#include "TemplateConditionCmdParser.hpp"

using namespace obotcha;

namespace gagira {

String _HtmlTemplate::RangeCommand = createString("range");
String _HtmlTemplate::IndexCommand = createString("index");
String _HtmlTemplate::WithCommand = createString("with");
String _HtmlTemplate::EndCommand = createString("end");
String _HtmlTemplate::DotCommand = createString(".");
String _HtmlTemplate::IfCommand = createString("if");
String _HtmlTemplate::ElseIfCommand = createString("else if");
String _HtmlTemplate::ElseCommand = createString("else");

//template like Go
//{{.}}
_HtmlTemplate::_HtmlTemplate(String html) {
    int index = html->find("{{");
    int start = 0;
    int status = ParseTag;
    items = createArrayList<HtmlTemplateItem>();

    while(index != -1) {
        if(start != index) {
            //all the value should be save to text item
            String text = html->subString(start,index - start);
            printf("text is %s,status is %d \n",text->toChars(),status);
            if(status == ParseIfContent ||
                status == ParseElseContent ||
                status == ParseElseIfContent) {
                mCurrentParser->addPrevContent(text);
            } else {
                HtmlTemplateItem item = createHtmlTemplateTextItem(text);
                items->add(item);
            }
        }

        start = index + 2;
        index = html->find("}}",start);
    
        String text = html->subString(start,index - start);
        //parse the command;
        HtmlTemplateItem item;
        status = doParse(text);

        start = index + 2;
        index = html->find("{{",start);
    }
    
    //remain data should save to textItem
    printf("remain start is %d \n",start);
    if(start < html->size() - 1) {
        String text = html->subString(start,html->size() - start);
        printf("remain text is %s \n",text->toChars());
        HtmlTemplateItem item = createHtmlTemplateTextItem(text);
        items->add(item);
    }
    //String text = html->subString(start,html->size() - start);
    //printf("remain text is %s \n",text->toChars());
    //HtmlTemplateItem item = createHtmlTemplateTextItem(text);
    //items->add(item);
}

int _HtmlTemplate::doParse(String command) {
    printf("command is %s \n",command->toChars());
    String cmd = command->trim();
    if(cmd->startsWith(".")) {
        mCurrentParser = createTemplateDotCmdParser();
        mCurrentParser->doParse(cmd->subString(1,cmd->size() - 1));
        items->add(mCurrentParser->getTemplateItem());
        return ParseTag;
    } else if(cmd->startsWithIgnoreCase(IfCommand)) {
        mCurrentParser = createTemplateConditionCmdParser();
        //mCurrentParser->updateStatus(st(TemplateConditionCmdParser)::ParseIf);
        mCurrentParser->doParse(
            cmd->subString(IfCommand->size(),cmd->size()- IfCommand->size()));
        return ParseIfContent;
    } else if(cmd->startsWithIgnoreCase(ElseCommand)) {
        //mCurrentParser->updateStatus(st(TemplateConditionCmdParser)::ParseElse);
        printf("cmd is else\n");
        if(cmd->contains(IfCommand)) {
            int index = cmd->indexOf(IfCommand);
            mCurrentParser->doParse(cmd->subString(index + 2,cmd->size() - index - 2));
        } else {
            Cast<TemplateConditionCmdParser>(mCurrentParser)->addFinalItem();
        }
        return ParseElseContent;
    } else if(cmd->startsWithIgnoreCase(EndCommand)) {
        items->add(mCurrentParser->getTemplateItem());
        mCurrentParser = nullptr;
        return ParseTag;
    }
    //TODO
    return ParseTag;
}

String _HtmlTemplate::execute(Object data) {
    StringBuffer templateString = createStringBuffer();
    auto iterator = items->getIterator();
    printf("execute items size is %d \n",items->size());
    while(iterator->hasValue()) {
        auto item = iterator->getValue();
        templateString->append(item->toString(data));
        iterator->next();
    }

    return templateString->toString();
}


}
