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
    startAnalyse();
}

void _Template::startAnalyse() {
    int start = 0;
    int next = 0;
    printf("startAnalyse trace1 \n");
    while(1) {
        auto start_index = mText->find(StartTag,start);
        printf("startAnalyse trace1_1,start_index is %d \n",start_index);
        if(start_index == -1) {
            break;
        }

        auto end_index = mText->find(EndTag,start_index + 2);
        printf("startAnalyse trace1_2,end_index is %d \n",end_index);
        if(end_index == -1) {
            break;
        }
        
        printf("startAnalyse trace start_index is %d, end_index is %d\n",start_index,end_index);
        //save text before start_tag
        if(start != start_index) {
            TemplateTextItem item = TemplateTextItem::New(mText->subString(start,start_index - start));
            mItems->add(item);
        }

        start = end_index + 2;

        //start parse command
        /**
         * 
         *command like:
         *1.create value
         *  var t;
         *  var v = 100;
        */
        auto command = mText->subString(start_index + 2,end_index - start_index - 2)->trim();
        TextLineReader lineReader = TextLineReader::New(command);
        TemplateParserDispatcher dispatcher = TemplateParserDispatcher::New();
        printf("startAnalyse trace2 \n");
        TemplateParser currentParser = nullptr;

        while(1) {
            auto line = lineReader->readLine();
            if(line == nullptr) {
                printf("startAnalyse trace3 \n");
                break;
            }
            printf("startAnalyse trace4,line is %s \n",line->toChars());
            if(currentParser == nullptr) {
                currentParser = dispatcher->apply(line);
            } else {
                currentParser->inject(line);
            }

            auto item = currentParser->doParse();
            printf("startAnalyse trace5_1 \n");
            if(item == nullptr) {
                printf("startAnalyse trace5_2,it is null!!!! \n");
            }
            if(item != nullptr) {
                printf("startAnalyse trace6 \n");
                item->dump();
            }
        }


    }
}

}