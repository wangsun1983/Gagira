#include "HtmlTemplate.hpp"
#include "HtmlTemplateTextItem.hpp"
#include "TemplateDotCmdParser.hpp"
#include "StringBuffer.hpp"
#include "TemplateConditionCmdParser.hpp"
#include "TemplateReferCmdParser.hpp"

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
_HtmlTemplate::_HtmlTemplate() {
    items = createArrayList<HtmlTemplateItem>();
    mStatus = ParseTag;
}

int _HtmlTemplate::import(String html,int start,bool onsection) {
    tagStartIndex = start;
    tagEndIndex = start;
    while(tagStartIndex >= 0) {
        tagStartIndex = html->find("{{",tagEndIndex);
        printf("tagStartIndex is %d \n",tagStartIndex);
        if(tagStartIndex == -1) {
            //save remain text;
            if(tagEndIndex < html->size() - 1) {
                String text = nullptr;
                if(tagEndIndex > 0) {
                    text = html->subString(tagEndIndex + 2,html->size() - tagEndIndex - 2);
                } else {
                    text = html->subString(0,html->size());
                }
                
                if(text != nullptr && text->size() > 0) {
                    printf("add remain text,text is %s \n",text->toChars());
                    HtmlTemplateItem item = createHtmlTemplateTextItem(text);
                    items->add(item);
                }
            }
            return -1;
        }

        if(tagStartIndex < html->size() - 1) {
            String text = nullptr;
            if(tagEndIndex == 0) {
                text = html->subString(tagEndIndex,tagStartIndex - tagEndIndex); //first 
            } else {
                text = html->subString(tagEndIndex + 2,tagStartIndex - tagEndIndex - 2); //first
            }
            
            if(text != nullptr && text->size() != 0) {
                HtmlTemplateItem item = createHtmlTemplateTextItem(text);
                printf("add start text,text is %s \n",text->toChars());
                items->add(item);
            }
        }

LoopFind:
        tagEndIndex = html->find("}}",tagStartIndex);
        String cmd = html->subString(tagStartIndex + 2,tagEndIndex - tagStartIndex - 2);
        
        cmd = cmd->trim();
        if(cmd->startsWith("/*") && cmd->endsWith("*/")) {
            //this is a comment,Do nothing;
        } if(cmd->startsWith(".")) {
            mCurrentParser = createTemplateDotCmdParser();
            mCurrentParser->doParse(cmd->subString(1,cmd->size() - 1));
            items->add(mCurrentParser->getTemplateItem());
            mStatus = ParseTag;
        } else if(cmd->startsWithIgnoreCase(IfCommand)) {
            printf("_HtmlTemplate if command trace1 \n");
            mStatus = ParseIfContent;
            mCurrentParser = createTemplateConditionCmdParser();
            mCurrentParser->doParse(
                    cmd->subString(IfCommand->size(),cmd->size()- IfCommand->size()));
            HtmlTemplateConditionItem item = Cast<HtmlTemplateConditionItem>(mCurrentParser->getTemplateItem());
            HtmlTemplateCondition cond =item->conditions->get(item->conditions->size() - 1);
            cond->content = createHtmlTemplate();
            int ret = cond->content->import(html,tagEndIndex,true);
            printf("import trace1 ret is %d \n",ret);
            if(ret < 0) {
                tagStartIndex = cond->content->tagStartIndex;
                goto LoopFind;
            } else {
                tagStartIndex = ret;
            }
        } else if(cmd->startsWithIgnoreCase(ElseCommand)) {
            if(mStatus == ParseTag) {
                //wrong status,this may be form condition's else
                return -1;
            }

            if(cmd->contains(IfCommand)) {
                int index = cmd->indexOf(IfCommand);
                mCurrentParser->doParse(cmd->subString(index + 2,cmd->size() - index - 2));
            } else {
                Cast<TemplateConditionCmdParser>(mCurrentParser)->addFinalItem();
            }
            mStatus = ParseIfContent;

            HtmlTemplateConditionItem item = Cast<HtmlTemplateConditionItem>(mCurrentParser->getTemplateItem());
            HtmlTemplateCondition cond =item->conditions->get(item->conditions->size() - 1);
            cond->content = createHtmlTemplate();
            int ret = cond->content->import(html,tagEndIndex,true);
            printf("import trace2 ret is %d \n",ret);
            if(ret < 0) {
                tagStartIndex = cond->content->tagStartIndex;
                goto LoopFind;
            } else {
                tagStartIndex = ret;
            }
        } else if(cmd->startsWithIgnoreCase(EndCommand)) {
            printf("cmd is %s ,mStatus is %d\n",cmd->toChars(),mStatus);
            if(mStatus == ParseIfContent || mStatus == ParseElseContent ||mStatus == ParseElseIfContent) {
                items->add(mCurrentParser->getTemplateItem());
                printf("end!!!! tagEndIndex is %d \n",tagEndIndex);
            }

            if(onsection) {
                return  -1;
            }
        } else if(cmd->startsWith("$")) {
            mCurrentParser = createTemplateReferCmdParser();
            mCurrentParser->doParse(cmd->subString(1,cmd->size()- 1));
            items->add(mCurrentParser->getTemplateItem());
        }
    }

    printf("final tagEndIndex is %d,html size is %d \n",tagEndIndex,html->size());
    

    return 0;
}

String _HtmlTemplate::execute(Object data) {
    StringBuffer templateString = createStringBuffer();
    auto iterator = items->getIterator();
    printf("execute items size is %d \n",items->size());
    while(iterator->hasValue()) {
        auto item = iterator->getValue();
        
        String tempStr = item->toString(data);
        if(tempStr != nullptr) {
            templateString->append(tempStr);
            printf("item to string is : %s \n",item->toString(data)->toChars());
        }
        iterator->next();
    }

    return templateString->toString();
}


}
