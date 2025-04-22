#include "TemplateParserHelper.hpp"

using namespace obotcha;

namespace gagira {

ArrayList<String>_TemplateParserHelper::ParseStatement(String cmd,char splitTag) {
    auto initStr = cmd->trim();
    auto size = initStr->size();
    ArrayList<String> statements = ArrayList<String>::New();

    int start = 0;

    if(size != 0) {
        int isInString = 0;
        auto initChars = initStr->toChars();
        for(int i = 0; i < size ;i++ ) {
            if(initChars[i] == '\\') {
                i++;
            } else if(initChars[i] == '\"') {
                isInString++;
                if(isInString % 2 == 0 && isInString != 0) {
                    isInString = 0;
                }
            } else if(initChars[i] == splitTag){
                auto statement = initStr->subString(start,i - start);
                statements->add(statement);
                i++;
                start = i;
            }
        }
    }

    if(start != cmd->size()) {
        auto statement = initStr->subString(start,initStr->size() - start);
        statements->add(statement);
    }

    return statements;
}

}
