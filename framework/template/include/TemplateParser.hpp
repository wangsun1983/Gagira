#ifndef __GAGRIA_TEMPLATE_PARSER_HPP__
#define __GAGRIA_TEMPLATE_PARSER_HPP__

#include "TemplateItem.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateParser) {
public:
    enum Type {
        Comment = 0,
        Condition,
        Function,
        Let,
        Print,
        Elif,
        Else,
        In,
        Loop,
        End
    };

    virtual void inject(String) {};
    virtual TemplateItem doParse() = 0;
    virtual bool processText(String) {
        return false;
    }

    int getType();
    void setType(int);

protected:
    int mType;
};

}

#endif
