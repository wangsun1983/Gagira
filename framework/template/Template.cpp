#include "Template.hpp"
#include "TemplateInstruction.hpp"
#include "TemplateTextItem.hpp"
#include "TextLineReader.hpp"
#include "ForEveryOne.hpp"
#include "TemplateParserDispatcher.hpp"

namespace gagira {

_Template::_Template(String text) {
    mItems = ArrayList<TemplateItem>::New();
    mText = text;
    mStatus = ParseHtml;
    startAnalyse();
}

void _Template::startAnalyse() {
    TextLineReader lineReader = TextLineReader::New(mText);
    TemplateParser parser = nullptr;

    while(1) {
        auto line = lineReader->readLine();
        if(line == nullptr) {
            break;
        }
        
        if(mStatus == ParseCommand) {
            //line = line->trim();
            line = filter(line);
        }

        if(line->size() == 0) {
            continue;
        }
        int start_index = 0;
        int end_index = 0;
        while(start_index < line->size()) {
            switch(mStatus) {
                case ParseHtml: {
                    auto next_index = line->find(StartTag,start_index);
                    if(next_index != -1) {
                        if(next_index != start_index) {
                            auto content = line->subString(start_index,next_index - start_index);
                            if(parser == nullptr || !parser->processText(content)) {
                                if(start_index != next_index) {
                                    mItems->add(TemplateTextItem::New(content));
                                }
                            }
                        }
                        start_index = next_index + StartTag->size();
                        mStatus = ParseCommand;
                    } else {
                        auto content = line->subString(start_index,line->size() - start_index)->append("\n");
                        if(parser == nullptr || !parser->processText(content)) {
                            mItems->add(TemplateTextItem::New(content));
                        }
                        start_index = line->size();
                    }
                } break;

                case ParseCommand: {
                    int end_index = line->find(EndTag,start_index);
                    String commands = nullptr;
                    if(end_index != -1) {
                        if(end_index != start_index) {
                            commands = line->subString(start_index,end_index - start_index - 2);
                        }
                        start_index = end_index + 2;
                        mStatus = ParseHtml;
                    } else {
                        commands = line->subString(start_index,line->size() - start_index);
                        start_index = line->size();
                    }

                    if(commands != nullptr) {
                        if(parser == nullptr) {
                            parser = st(TemplateParserDispatcher)::Apply(commands);
                        } else {
                            parser->inject(commands);
                        }

                        auto item = parser->doParse();
                        if(item != nullptr) {
                            mItems->add(item);
                            parser = nullptr;
                        }
                    }
                } break;
            }
        }
    }
}

String _Template::execute(HashMap<String,TemplateScopedValue> scopedValues,Object obj) {
    String result = "";
    mScopedValueContainer = TemplateScopedValueContainer::New(scopedValues);
    mObjectContainer = TemplateObjectContainer::New(obj);
    ForEveryOne(item,mItems) {
        auto v = item->execute(mScopedValueContainer,mObjectContainer);
        if(v != nullptr) {
            if(v->isDirectReturn()) {
                return v->toString();
            }
            result = result->append(v->toString());
        }
    }

    return result;
}

String _Template::filter(String in) {
    //remove tab
    return in->replaceAll(String::New("\t"),String::New(" "))->trim();
}

}