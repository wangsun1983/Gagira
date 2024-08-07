#include "HtmlTemplate.hpp"
#include "HtmlTemplateTextItem.hpp"
#include "TemplateDotCmdParser.hpp"
#include "StringBuffer.hpp"
#include "TemplateConditionCmdParser.hpp"
#include "TemplateReferCmdParser.hpp"
#include "TemplateRangeCmdParser.hpp"
#include "TemplateIncludeCmdParser.hpp"
#include "TemplateDefineCmdParser.hpp"
#include "TemplateLenCmdParser.hpp"
#include "TemplateWithCmdParser.hpp"
#include "TemplateFunctionCmdParser.hpp"
#include "FileInputStream.hpp"
#include "Log.hpp"

using namespace obotcha;

namespace gagira {

String _HtmlTemplate::RangeCommand = String::New("range");
String _HtmlTemplate::IndexCommand = String::New("index");
String _HtmlTemplate::WithCommand = String::New("with");
String _HtmlTemplate::EndCommand = String::New("end");
String _HtmlTemplate::DotCommand = String::New(".");
String _HtmlTemplate::IfCommand = String::New("if");
String _HtmlTemplate::ElseIfCommand = String::New("else if");
String _HtmlTemplate::ElseCommand = String::New("else");
String _HtmlTemplate::IncludeCommand = String::New("template");
String _HtmlTemplate::DefineCommand = String::New("define");
String _HtmlTemplate::LenCommand = String::New("len");

//template like Go
//{{.}}
_HtmlTemplate::_HtmlTemplate() {
    items = ArrayList<HtmlTemplateItem>::New();
    sections = HashMap<String,HtmlTemplateItem>::New();
    //mFunctions = HashMap<String,HtmlTemplateItem>::New();
    functionsCache = ArrayList<Object>::New();

    memset(mStatus,0,32*sizeof(int));
    mStatusCount = 0;
    mStatus[0] = ParseTag;
}

void _HtmlTemplate::clear() {
    items->clear();
    sections->clear();
    functionsCache->clear();
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
                    if(html->size() - tagEndIndex - 2 > 0) {
                        text = html->subString(tagEndIndex + 2,html->size() - tagEndIndex - 2);
                    }
                } else {
                    text = html->subString(0,html->size());
                }
                
                if(text != nullptr && text->size() > 0) {
                    HtmlTemplateItem item = HtmlTemplateTextItem::New(text);
                    items->add(item);
                }
            }
            return -1;
        }

        if(tagStartIndex < html->size() - 1) {
            String text = nullptr;
            if(tagEndIndex == 0) {
                if(tagStartIndex - tagEndIndex > 0) {
                    text = html->subString(tagEndIndex,tagStartIndex - tagEndIndex); //first 
                }
            } else {
                text = html->subString(tagEndIndex + 2,tagStartIndex - tagEndIndex - 2); //first
            }
            
            if(text != nullptr && text->size() != 0) {
                HtmlTemplateItem item = HtmlTemplateTextItem::New(text);
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
            mCurrentParser = TemplateDotCmdParser::New();
            mCurrentParser->doParse(cmd->subString(1,cmd->size() - 1));
            items->add(mCurrentParser->getTemplateItem());
        } else if(cmd->startsWithIgnoreCase(IfCommand)) {
            //mStatus = ParseIfContent;
            addCurrentStatus(ParseIfContent);
            mCurrentParser = TemplateConditionCmdParser::New();
            mCurrentParser->doParse(
                    cmd->subString(IfCommand->size(),cmd->size()- IfCommand->size()));
            HtmlTemplateConditionItem item = Cast<HtmlTemplateConditionItem>(mCurrentParser->getTemplateItem());
            HtmlTemplateCondition cond =item->conditions->get(item->conditions->size() - 1);
            cond->content = HtmlTemplate::New();
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
                cond->content = HtmlTemplate::New();
                newTemplate = cond->content;
            } else if(currentStatus() == ParseRangeContent) {
                HtmlTemplateRangeItem item = Cast<HtmlTemplateRangeItem>(mCurrentParser->getTemplateItem());
                if(item == nullptr) {
                    return -1;
                }
                item->mNoValueTemplate = HtmlTemplate::New();
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
                status == ParseElseIfContent || status == ParseRangeContent||
                status == ParseWith) {
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
            mCurrentParser = TemplateReferCmdParser::New();
            mCurrentParser->doParse(cmd->subString(1,cmd->size()- 1));
            items->add(mCurrentParser->getTemplateItem());
        } else if(cmd->startsWith(LenCommand)) {
            mCurrentParser = TemplateLenCmdParser::New();
            mCurrentParser->doParse(cmd->subString(LenCommand->size(),cmd->size()- LenCommand->size()));
            items->add(mCurrentParser->getTemplateItem());
        } else if(cmd->startsWithIgnoreCase(RangeCommand)) {
            addCurrentStatus(ParseRangeContent);
            mCurrentParser = TemplateRangeCmdParser::New();
            mCurrentParser->doParse(cmd->subString(RangeCommand->size(),cmd->size()- RangeCommand->size()));
            
            HtmlTemplateRangeItem item = Cast<HtmlTemplateRangeItem>(mCurrentParser->getTemplateItem());
            item->mTemplate = HtmlTemplate::New();
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
            mCurrentParser = TemplateIncludeCmdParser::New();
            mCurrentParser->doParse(cmd->subString(IncludeCommand->size(),cmd->size()- IncludeCommand->size()));
            items->add(mCurrentParser->getTemplateItem());
        } else if(cmd->startsWithIgnoreCase(DefineCommand)) {
            addCurrentStatus(ParseDefine);
            mCurrentParser = TemplateDefineCmdParser::New();
            mCurrentParser->doParse(cmd->subString(DefineCommand->size(),cmd->size()- DefineCommand->size()));
            
            HtmlTemplateDefineItem item = Cast<HtmlTemplateDefineItem>(mCurrentParser->getTemplateItem());
            item->mTemplate = HtmlTemplate::New();
            item->mTemplate->addCurrentStatus(ParseDefine);
            int ret = item->mTemplate->import(html,tagEndIndex,true);
            //removeCurrentStatus();
            if(ret < 0) {
                tagStartIndex = item->mTemplate->tagStartIndex;
                goto LoopFind;
            } else {
                tagStartIndex = ret;
            }
        } else if(cmd->startsWithIgnoreCase(WithCommand)) {
            addCurrentStatus(ParseWith);
            mCurrentParser = TemplateWithCmdParser::New();
            mCurrentParser->doParse(cmd->subString(WithCommand->size(),cmd->size()- WithCommand->size()));
            HtmlTemplateWithItem item = Cast<HtmlTemplateWithItem>(mCurrentParser->getTemplateItem());
            item->content = HtmlTemplate::New();
            int ret = item->content->import(html,tagEndIndex,true);
            //removeCurrentStatus();
            if(ret < 0) {
                tagStartIndex = item->content->tagStartIndex;
                goto LoopFind;
            } else {
                tagStartIndex = ret;
            }
        } else {
            //this may be a function item
            int index = cmd->indexOf(" ");
            String name = cmd->subString(0,index);
            if(mFunctions.find(name->getStdString()) != mFunctions.end()) {
                mCurrentParser = TemplateFunctionCmdParser::New(name,mFunctions[name->getStdString()]);
                mCurrentParser->doParse(cmd->subString(name->size(),cmd->size()- name->size()));
                items->add(mCurrentParser->getTemplateItem());
            }
        }
    }


    return 0;
}

String _HtmlTemplate::execute(Object data) {
    StringBuffer templateString = StringBuffer::New();
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
    FileInputStream input = FileInputStream::New(path);
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

ArrayList<HtmlTemplateItem> _HtmlTemplate::getItems() {
    return items;
}

void _HtmlTemplate::setTemplateFunc(String name,HtmlTemplateFunction func) {
    //mFunctions->put(name,func);
    mFunctions[name->getStdString()] = func;
}

void _HtmlTemplate::saveFuncObjCache(Object o) {
    functionsCache->add(o);
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
