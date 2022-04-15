#include "HtmlTemplate.hpp"
#include "HtmlTemplateTextItem.hpp"
#include "TemplateDotCmdParser.hpp"
#include "StringBuffer.hpp"
#include "TemplateConditionCmdParser.hpp"
#include "TemplateReferCmdParser.hpp"
#include "TemplateRangeCmdParser.hpp"
#include "TemplateIncludeCmdParser.hpp"
#include "TemplateDefineCmdParser.hpp"
#include "FileInputStream.hpp"
#include "Log.hpp"

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
String _HtmlTemplate::IncludeCommand = createString("template");
String _HtmlTemplate::DefineCommand = createString("define");

//template like Go
//{{.}}
_HtmlTemplate::_HtmlTemplate() {
    items = createArrayList<HtmlTemplateItem>();
    sections = createHashMap<String,HtmlTemplateItem>();
    memset(mStatus,0,32*sizeof(int));
    mStatusCount = 0;
    mStatus[0] = ParseTag;
}

int _HtmlTemplate::import(String html,int start,bool onsection) {
    tagStartIndex = start;
    tagEndIndex = start;
    while(tagStartIndex >= 0) {
        tagStartIndex = html->find("{{",tagEndIndex);
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
        } else if(cmd->startsWithIgnoreCase(IfCommand)) {
            //mStatus = ParseIfContent;
            addCurrentStatus(ParseIfContent);
            mCurrentParser = createTemplateConditionCmdParser();
            mCurrentParser->doParse(
                    cmd->subString(IfCommand->size(),cmd->size()- IfCommand->size()));
            HtmlTemplateConditionItem item = Cast<HtmlTemplateConditionItem>(mCurrentParser->getTemplateItem());
            HtmlTemplateCondition cond =item->conditions->get(item->conditions->size() - 1);
            cond->content = createHtmlTemplate();
            cond->content->addCurrentStatus(ParseIfContent);
            int ret = cond->content->import(html,tagEndIndex,true);
            //removeCurrentStatus();
            if(ret < 0) {
                tagStartIndex = cond->content->tagStartIndex;
                goto LoopFind;
            } else {
                tagStartIndex = ret;
            }
        } else if(cmd->startsWithIgnoreCase(ElseCommand)) {
            if(currentStatus() == ParseTag) {
                //wrong status,this may be form condition's else
                return -1;
            }

            HtmlTemplate newTemplate = nullptr;
            if(currentStatus() == ParseIfContent) {
                if(cmd->contains(IfCommand)) {
                    int index = cmd->indexOf(IfCommand);
                    mCurrentParser->doParse(cmd->subString(index + 2,cmd->size() - index - 2));
                } else {
                    auto parser = Cast<TemplateConditionCmdParser>(mCurrentParser);
                    if(parser == nullptr) {
                        return -1;
                    }
                    parser->addFinalItem();
                }
                HtmlTemplateConditionItem item = Cast<HtmlTemplateConditionItem>(mCurrentParser->getTemplateItem());
                HtmlTemplateCondition cond =item->conditions->get(item->conditions->size() - 1);
                cond->content = createHtmlTemplate();
                newTemplate = cond->content;
            } else if(currentStatus() == ParseRangeContent) {
                HtmlTemplateRangeItem item = Cast<HtmlTemplateRangeItem>(mCurrentParser->getTemplateItem());
                if(item == nullptr) {
                    return -1;
                }
                item->mNoValueTemplate = createHtmlTemplate();
                newTemplate = item->mNoValueTemplate;
            }
            
            int ret = newTemplate->import(html,tagEndIndex,true);
            if(ret < 0) {
                tagStartIndex = newTemplate->tagStartIndex;
                goto LoopFind;
            } else {
                tagStartIndex = ret;
            }
        } else if(cmd->startsWithIgnoreCase(EndCommand)) {
            int status = currentStatus();
            if (status == ParseIfContent || status == ParseElseContent ||
                status == ParseElseIfContent || status == ParseRangeContent) {
                if (mCurrentParser != nullptr) {
                    items->add(mCurrentParser->getTemplateItem());
                }
                removeCurrentStatus();
            } else if (status == ParseDefine) {
                HtmlTemplateDefineItem item = Cast<HtmlTemplateDefineItem>(mCurrentParser->getTemplateItem());
                removeCurrentStatus();
                if(item != nullptr) {
                    sections->put(item->mName,item);
                }
            }

            if(onsection && mStatusCount == 0) {
                return  -1;
            }
        } else if(cmd->startsWith("$")) {
            mCurrentParser = createTemplateReferCmdParser();
            mCurrentParser->doParse(cmd->subString(1,cmd->size()- 1));
            items->add(mCurrentParser->getTemplateItem());
        } else if(cmd->startsWithIgnoreCase(RangeCommand)) {
            addCurrentStatus(ParseRangeContent);
            mCurrentParser = createTemplateRangeCmdParser();
            mCurrentParser->doParse(cmd->subString(RangeCommand->size(),cmd->size()- RangeCommand->size()));
            
            HtmlTemplateRangeItem item = Cast<HtmlTemplateRangeItem>(mCurrentParser->getTemplateItem());
            item->mTemplate = createHtmlTemplate();
            item->mTemplate->addCurrentStatus(ParseRangeContent);
            int ret = item->mTemplate->import(html,tagEndIndex,true);
            //removeCurrentStatus();
            if(ret < 0) {
                tagStartIndex = item->mTemplate->tagStartIndex;
                goto LoopFind;
            } else {
                tagStartIndex = ret;
            }
        } else if(cmd->startsWithIgnoreCase(IncludeCommand)) {
            mCurrentParser = createTemplateIncludeCmdParser();
            mCurrentParser->doParse(cmd->subString(IncludeCommand->size(),cmd->size()- IncludeCommand->size()));
            items->add(mCurrentParser->getTemplateItem());
        } else if(cmd->startsWithIgnoreCase(DefineCommand)) {
            addCurrentStatus(ParseDefine);
            mCurrentParser = createTemplateDefineCmdParser();
            mCurrentParser->doParse(cmd->subString(DefineCommand->size(),cmd->size()- DefineCommand->size()));
            
            HtmlTemplateDefineItem item = Cast<HtmlTemplateDefineItem>(mCurrentParser->getTemplateItem());
            item->mTemplate = createHtmlTemplate();
            item->mTemplate->addCurrentStatus(ParseDefine);
            int ret = item->mTemplate->import(html,tagEndIndex,true);
            //removeCurrentStatus();
            if(ret < 0) {
                tagStartIndex = item->mTemplate->tagStartIndex;
                goto LoopFind;
            } else {
                tagStartIndex = ret;
            }
        }
    }


    return 0;
}

String _HtmlTemplate::execute(Object data) {
    StringBuffer templateString = createStringBuffer();
    auto iterator = items->getIterator();
    while(iterator->hasValue()) {
        auto item = iterator->getValue();
        
        String tempStr = item->toString(data);
        if(tempStr != nullptr) {
            templateString->append(tempStr);
        }
        iterator->next();
    }

    return templateString->toString();
}

int _HtmlTemplate::importFile(String path) {
    FileInputStream input = createFileInputStream(path);
    input->open();
    String content = input->readAll()->toString();
    input->close();
    return this->import(content);
}

String _HtmlTemplate::execute(String section,Object data) {
    HtmlTemplateDefineItem item = Cast<HtmlTemplateDefineItem>(sections->get(section));
    if(item == nullptr) {
        return nullptr;
    }

    return item->toString(data);
}

int _HtmlTemplate::currentStatus() {
    return mStatus[mStatusCount];
}

void _HtmlTemplate::addCurrentStatus(int s) {
    mStatusCount++;
    mStatus[mStatusCount] = s;    
}

void _HtmlTemplate::removeCurrentStatus() {
    if(mStatusCount == 0) {
        LOG(ERROR)<<"removeCurrentStatus error!!!";
        return;
    }
    mStatusCount--;
}

}
