#ifndef __GAGRIA_TEMPLATE_LET_PARSER_HPP__
#define __GAGRIA_TEMPLATE_LET_PARSER_HPP__

#include "TemplateParser.hpp"
#include "TemplateInstruction.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateLetParser) IMPLEMENTS(TemplateParser,TemplateInstruction){
public:
    _TemplateLetParser(String cmd);
    TemplateItem doParse();

private:
    enum ParserStatus {
        ParseType = 0,
        ParseScopedName,
        ParseScopdeValue
    };

    String mCmd;
    int mStatus;

    int mParsedType;
    String mParsedName;
};

}

#endif
