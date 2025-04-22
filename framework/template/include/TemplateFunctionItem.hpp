#ifndef __GAGRIA_TEMPLATE_FUNCTION_ITEM_HPP__
#define __GAGRIA_TEMPLATE_FUNCTION_ITEM_HPP__

#include "String.hpp"
#include "TemplateItem.hpp"
#include "Template.hpp"
#include "ArrayList.hpp"
#include "TemplateScopedValue.hpp"
#include "TemplateScopedValueContainer.hpp"

using namespace obotcha;

namespace gagira {

//----- HtmlTemplateFuncExpression ----- 
DECLARE_CLASS(TemplateFuncExpression) {
public:
    enum Type {
        Variable = 0,
        Function,
        Operator
    };

    TemplateScopedValue getContent();
    ArrayList<String> getParams();
    String getOperator();
    int getType();

    virtual TemplateScopedValue execute(TemplateScopedValueContainer,TemplateObjectContainer objContainer) {
        return nullptr;
    }

    virtual void dump() {};

protected:
    int mType;
    TemplateScopedValue mContent;
    ArrayList<String> mParams;    
    String mOperator;
};

DECLARE_CLASS(TemplateFuncExpression_Variable) IMPLEMENTS(TemplateFuncExpression,TemplateInstruction) {
public:
    _TemplateFuncExpression_Variable(TemplateScopedValue c);
    void dump();
    //TemplateScopedValue execute(HashMap<String,TemplateScopedValue> scopedValues,Object obj);
    TemplateScopedValue execute(TemplateScopedValueContainer container,TemplateObjectContainer objContainer);
};

DECLARE_CLASS(TemplateFuncExpression_Function) IMPLEMENTS(TemplateFuncExpression) {
public:
    _TemplateFuncExpression_Function(String name,ArrayList<String> params);
    _TemplateFuncExpression_Function(String name);
    TemplateScopedValue execute(TemplateScopedValueContainer container,TemplateObjectContainer objContainer);
    void dump();
};

DECLARE_CLASS(TemplateFuncExpression_Operator) IMPLEMENTS(TemplateFuncExpression) {
public:
    _TemplateFuncExpression_Operator(String opratorName);
    void dump();
};

//----- HtmlTemplateFunctionItem ----- 
DECLARE_CLASS(TemplateFunctionItem) IMPLEMENTS(TemplateItem) {

public:
    _TemplateFunctionItem(ArrayList<TemplateFuncExpression>);
    TemplateScopedValue execute(TemplateScopedValueContainer,TemplateObjectContainer);

private:
    ArrayList<TemplateFuncExpression> mExpressions;

    // int execute_for_int(HashMap<String,TemplateScopedValue> scopedValues,Object obj);
    // String execute_for_string(HashMap<String,TemplateScopedValue> scopedValues,Object obj);

    //operator:=
    TemplateScopedValue computeAssignment(String tag,TemplateScopedValue value,
                                        TemplateScopedValueContainer container,TemplateObjectContainer objContainer);

    //operator:#-
    TemplateScopedValue computeNegative(TemplateScopedValue arg1);

    //operator:==
    TemplateScopedValue computeEqual(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:!=
    TemplateScopedValue computeNotEqual(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:~
    TemplateScopedValue computeReverse(TemplateScopedValue arg1);

    //operator:--
    TemplateScopedValue computeMinusMinus(TemplateScopedValue arg1);

    //operator:++
    TemplateScopedValue computePlusPlus(TemplateScopedValue arg1);

    //operator:+
    TemplateScopedValue computePlus(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:-
    TemplateScopedValue computeMinus(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:*
    TemplateScopedValue computeMultiple(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:/
    TemplateScopedValue computeDivide(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:%
    TemplateScopedValue computeModular(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:<<
    TemplateScopedValue computeLeftShift(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:>>
    TemplateScopedValue computeRightShift(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:|
    TemplateScopedValue computeOrOperation(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:&
    TemplateScopedValue computeAndOperation(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:>
    TemplateScopedValue computeMoreThan(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:<
    TemplateScopedValue computeLessThan(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:<=
    TemplateScopedValue computeLessThanOrEqual(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operatpr:>=
    TemplateScopedValue computeMoreThanOrEqual(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:&&
    TemplateScopedValue computeAndCondition(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:||
    TemplateScopedValue computeOrCondition(TemplateScopedValue arg1,TemplateScopedValue arg2);

    //operator:!
    TemplateScopedValue computeExclamation(TemplateScopedValue arg1);
};

}

#endif
