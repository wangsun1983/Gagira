#include "TemplateFunctionItem.hpp"
#include "ForEveryOne.hpp"
#include "Field.hpp"
#include "Log.hpp"
#include "IllegalArgumentException.hpp"
#include "MethodNotSupportException.hpp"
#include "TemplateScopedValueSeeker.hpp"

using namespace obotcha;

namespace gagira {

//----- TemplateFuncExpression ----- 
TemplateScopedValue _TemplateFuncExpression::getContent() {
    return mContent;
}

ArrayList<String> _TemplateFuncExpression::getParams() {
    return mParams;
}

int _TemplateFuncExpression::getType() {
    return mType;
}

String _TemplateFuncExpression::getOperator() {
    return mOperator;
}

//----- TemplateFuncExpression_Variable ----- 
_TemplateFuncExpression_Variable::_TemplateFuncExpression_Variable(TemplateScopedValue c) {
    mContent = c;
    mType = Type::Variable;
}

TemplateScopedValue _TemplateFuncExpression_Variable::execute(TemplateScopedValueContainer container,TemplateObjectContainer objContainer) {
    if(mContent->getType() == st(TemplateScopedValue)::Type::String) {
        printf("TemplateFuncExpression_Variable execute trace1,unit name is %s,type is %d \n",mContent->getStringValue()->toChars(),mContent->getType());
        String mUnitName = mContent->getStringValue();
        if(mUnitName->sameAs(TrueStatement) || mUnitName->sameAs(FalseStatement)) {
            auto bool_value = st(Boolean)::Parse(mUnitName);
            return TemplateScopedValue::New(bool_value);
        } else if(mUnitName->startsWith("\"") && mUnitName->endsWith("\"")) {
            return TemplateScopedValue::New(mUnitName->subString(1,mUnitName->size() - 2));
        } else if(mUnitName->contains(".")) {
            //find object
            printf("TemplateFuncExpression_Variable execute trace1_2 \n");
            auto scopedValue = st(TemplateScopedValueSeeker)::Get(mUnitName,container,objContainer);
            if(scopedValue != nullptr) {
                return scopedValue;
            }
        }
        printf("TemplateFuncExpression_Variable execute trace1_3 \n");
        auto value = container->getScopedValue(mUnitName);
        if(value != nullptr) {
            return value;
        }
    }
    printf("TemplateFuncExpression_Variable execute trace2 \n");
    return mContent;
}

void _TemplateFuncExpression_Variable::dump() {
    // if(mContent != nullptr) {
    //     if(mContent->getStringValue() != nullptr) {
    //         printf("Expression Type:Variable,Its name is [%s] \n",mContent->getStringValue()->toChars());
    //     } else {
    //         printf("Expression Type:Variable,Its name is [%d] \n",mContent->getIntValue());
    //     }
    // }
}

//----- TemplateFuncExpression_Function ----- 
_TemplateFuncExpression_Function::_TemplateFuncExpression_Function(String name,ArrayList<String> params) {
    //mUnitName = name;
    mParams = params;
    mType = Type::Function;
}

_TemplateFuncExpression_Function::_TemplateFuncExpression_Function(String name) {
    //mUnitName = name;
    mType = Type::Function;
}

void _TemplateFuncExpression_Function::dump() {
    // printf("Expression Type:Function,Its name is [%s],its param is  ",mUnitName->toChars());
    // if(mParams == nullptr || mParams->size() == 0) {
    //     printf("None \n");
    // } else {
    //     ForEveryOne(param,mParams) {
    //         printf("%s ",param->toChars());
    //     }
    //     printf("\n");
    //}
}

TemplateScopedValue _TemplateFuncExpression_Function::execute(TemplateScopedValueContainer,TemplateObjectContainer obj) {
    //TODO
    //find function 
    return nullptr;
}

//----- TemplateFuncExpression_Operator ----- 
_TemplateFuncExpression_Operator::_TemplateFuncExpression_Operator(String opratorName) {
    mOperator = opratorName;
    mType = Type::Operator;
}

void _TemplateFuncExpression_Operator::dump() {
    //printf("Expression Type:Operator,Its name is [%s] \n",mOperator->toChars());
}


//----- TemplateFunctionItem ----- 
_TemplateFunctionItem::_TemplateFunctionItem(ArrayList<TemplateFuncExpression> expr) {
    mExpressions = expr;
}

TemplateScopedValue _TemplateFunctionItem::execute(TemplateScopedValueContainer scopedvalueContainer,TemplateObjectContainer objContainer) {
    //auto scopedValues = scopedvalueContainer->getCurrent();
    auto obj = objContainer->getCurrent();

#ifdef DEBUG_TEMPLATE_ITEM_COMMAND
    printf("[Execute Instruction]:[%s]\n",mCmd->toChars());
#endif

    String value = nullptr;
    LinkedList<TemplateFuncExpression> exprStack = LinkedList<TemplateFuncExpression>::New();
    ForEveryOne(expr,mExpressions) {
        if(expr->getType() == st(TemplateFuncExpression)::Type::Operator) {
#if DEBUG_EXPRESS_STACK            
            printf("-------------- \n");
            ForEveryOne(ex,exprStack) {
                printf("stack is %s \n",ex->getName()->toChars());
            }
            printf("-------------- \n");
            printf("\n");
#endif
            auto current = exprStack->takeLast();
            auto current_value = current->execute(scopedvalueContainer,objContainer);

            TemplateScopedValue compute_result = nullptr;

            auto m_operator = expr->getOperator();
            if(m_operator->sameAs("#-")) {
                compute_result = computeNegative(current_value);
            } else if(m_operator->sameAs("--")) {
                compute_result = computeMinusMinus(current_value);
            } else if(m_operator->sameAs("!")) {
                compute_result = computeExclamation(current_value);
            } else if(m_operator->sameAs("++")) {
                compute_result = computePlusPlus(current_value);
            } else if(m_operator->sameAs("~")) {
                compute_result = computeReverse(current_value);
            } else if(m_operator->sameAs("-")) {
                auto v = exprStack->takeLast();
                compute_result = computeMinus(v->execute(scopedvalueContainer,objContainer),current_value);
            } else if(m_operator->sameAs("+")) {
                auto v = exprStack->takeLast();
                compute_result = computePlus(current_value,v->execute(scopedvalueContainer,objContainer));
            } else if(m_operator->sameAs("*")) {
                auto v = exprStack->takeLast();
                compute_result = computeMultiple(current_value,v->execute(scopedvalueContainer,objContainer));
            } else if(m_operator->sameAs("/")) {
                auto v = exprStack->takeLast();
                compute_result = computeDivide(current_value,v->execute(scopedvalueContainer,objContainer));
            } else if(m_operator->sameAs("%")) {
                auto v = exprStack->takeLast();
                compute_result = computeModular(current_value,v->execute(scopedvalueContainer,objContainer));
            } else if(m_operator->sameAs("<<")) {
                auto v = exprStack->takeLast();
                compute_result = computeLeftShift(v->execute(scopedvalueContainer,objContainer),current_value);
            } else if(m_operator->sameAs(">>")) {
                auto v = exprStack->takeLast();
                compute_result = computeRightShift(v->execute(scopedvalueContainer,objContainer),current_value);
            } else if(m_operator->sameAs("|")) {
                auto v = exprStack->takeLast();
                compute_result = computeOrOperation(current_value,v->execute(scopedvalueContainer,objContainer));
            } else if(m_operator->sameAs("&")) {
                auto v = exprStack->takeLast();
                compute_result = computeAndOperation(current_value,v->execute(scopedvalueContainer,objContainer));
            } else if(m_operator->sameAs("==")) {
                auto v = exprStack->takeLast();
                compute_result = computeEqual(current_value,v->execute(scopedvalueContainer,objContainer));
            } else if(m_operator->sameAs("!=")) {
                auto v = exprStack->takeLast();
                compute_result = computeNotEqual(current_value,v->execute(scopedvalueContainer,objContainer));
            } else if(m_operator->sameAs(">")) {
                auto v = exprStack->takeLast();
                compute_result = computeMoreThan(v->execute(scopedvalueContainer,objContainer),current_value);
            } else if(m_operator->sameAs(">=")) {
                auto v = exprStack->takeLast();
                compute_result = computeMoreThanOrEqual(v->execute(scopedvalueContainer,objContainer),current_value);
            } else if(m_operator->sameAs("<")) {
                auto v = exprStack->takeLast();
                compute_result = computeLessThan(v->execute(scopedvalueContainer,objContainer),current_value);
            } else if(m_operator->sameAs("<=")) {
                auto v = exprStack->takeLast();
                compute_result = computeLessThanOrEqual(v->execute(scopedvalueContainer,objContainer),current_value);
            } else if(m_operator->sameAs("||")) {
                auto v = exprStack->takeLast();
                compute_result = computeOrCondition(v->execute(scopedvalueContainer,objContainer),current_value);
            } else if(m_operator->sameAs("&&")) {
                auto v = exprStack->takeLast();
                compute_result = computeAndCondition(v->execute(scopedvalueContainer,objContainer),current_value);
            } else if(m_operator->sameAs("=")) {
                auto v = exprStack->takeLast();
                compute_result = computeAssignment(scopedvalueContainer,obj,v->getContent()->getStringValue()->trim(),current_value);
            }

            if(compute_result != nullptr) {
                exprStack->putLast(TemplateFuncExpression_Variable::New(compute_result));
            }
        } else {
            exprStack->putLast(expr);
        }
    }

    if(exprStack->size() != 1) {
        LOG(ERROR)<<"execute_for_int,exprStack size is "<<exprStack->size();
    }

    //last value must be a value.
    auto last_value = exprStack->takeLast();
    if(last_value == nullptr) {
        return nullptr;
    }

    return last_value->execute(scopedvalueContainer,objContainer);
}

TemplateScopedValue _TemplateFunctionItem::computeEqual(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);

    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Bool: {
            return TemplateScopedValue::New(Boolean::New(arg1->getBoolValue() == arg2->getBoolValue()));
        } break;

        case st(TemplateScopedValue)::Type::Integer: {
            return TemplateScopedValue::New(Boolean::New(arg1->getIntValue() == arg2->getIntValue()));
        } break;

        case st(TemplateScopedValue)::Type::Double: {
            return TemplateScopedValue::New(Boolean::New(st(Double)::Compare(arg1->getBoolValue(),arg2->getBoolValue())));
        } break;

        case st(TemplateScopedValue)::Type::String: {
            return TemplateScopedValue::New(Boolean::New(arg1->getStringValue()->sameAs(arg2->getStringValue())));
        } break;
    }

    Trigger(MethodNotSupportException)
}

TemplateScopedValue _TemplateFunctionItem::computeNegative(TemplateScopedValue arg1) {
    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            auto value = arg1->getIntValue();
            return TemplateScopedValue::New(Integer::New(-value));
        } break;

        case st(TemplateScopedValue)::Type::Double: {
            auto value = arg1->getDoubleValue();
            return TemplateScopedValue::New(Double::New(-value));
        } break;

        case st(TemplateScopedValue)::Type::Bool:
        case st(TemplateScopedValue)::Type::String:
        break;
    }
    Trigger(IllegalArgumentException);
}

TemplateScopedValue _TemplateFunctionItem::computeNotEqual(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    auto result = computeEqual(arg1,arg2);
    result->updateBoolValue(!result->getBoolValue());
    return result;
}

TemplateScopedValue _TemplateFunctionItem::computeReverse(TemplateScopedValue arg1) {
    Panic(arg1->getType() != st(TemplateScopedValue)::Type::Integer,IllegalArgumentException);
    auto value = arg1->getIntValue();
    return TemplateScopedValue::New(Integer::New(~value));
}

//operator:--
TemplateScopedValue _TemplateFunctionItem::computeMinusMinus(TemplateScopedValue arg1) {
    Panic(arg1->getType() != st(TemplateScopedValue)::Type::Integer,IllegalArgumentException);
    auto value = arg1->getIntValue() - 1;
    return TemplateScopedValue::New(Integer::New(value));
}

//operator:++
TemplateScopedValue _TemplateFunctionItem::computePlusPlus(TemplateScopedValue arg1) {
    Panic(arg1->getType() != st(TemplateScopedValue)::Type::Integer,IllegalArgumentException);
    auto value = arg1->getIntValue() + 1;
    return TemplateScopedValue::New(Integer::New(value));
}

//operator:+
TemplateScopedValue _TemplateFunctionItem::computePlus(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);

    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            auto value = arg1->getIntValue() + arg2->getIntValue();
            return TemplateScopedValue::New(Integer::New(value));
        }

        case st(TemplateScopedValue)::Type::Double: {
            auto value = arg1->getDoubleValue() + arg2->getIntValue();
            return TemplateScopedValue::New(Double::New(value));
        }

        case st(TemplateScopedValue)::Type::String: {
            auto value = arg1->getStringValue()->append(arg2->getStringValue());
            return TemplateScopedValue::New(value);
        }

        case st(TemplateScopedValue)::Type::Bool:
        break;
    }

    Trigger(IllegalArgumentException);
}

//operator:-
TemplateScopedValue _TemplateFunctionItem::computeMinus(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);

    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            auto value = arg1->getIntValue() - arg2->getIntValue();
            return TemplateScopedValue::New(Integer::New(value));
        }

        case st(TemplateScopedValue)::Type::Double: {
            auto value = arg1->getDoubleValue() - arg2->getIntValue();
            return TemplateScopedValue::New(Double::New(value));
        }

        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }

    Trigger(IllegalArgumentException);
}

//operator:*
TemplateScopedValue _TemplateFunctionItem::computeMultiple(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);

    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            auto value = arg1->getIntValue() * arg2->getIntValue();
            return TemplateScopedValue::New(Integer::New(value));
        }

        case st(TemplateScopedValue)::Type::Double: {
            auto value = arg1->getDoubleValue() * arg2->getIntValue();
            return TemplateScopedValue::New(Double::New(value));
        }

        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }

    Trigger(IllegalArgumentException);
}

//operator:/
TemplateScopedValue _TemplateFunctionItem::computeDivide(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);

    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            auto value = arg1->getIntValue() / arg2->getIntValue();
            return TemplateScopedValue::New(Integer::New(value));
        }

        case st(TemplateScopedValue)::Type::Double: {
            auto value = arg1->getDoubleValue() / arg2->getIntValue();
            return TemplateScopedValue::New(Double::New(value));
        }

        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }

    Trigger(IllegalArgumentException);
}

//operator:%
TemplateScopedValue _TemplateFunctionItem::computeModular(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);

    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            auto value = arg1->getIntValue() % arg2->getIntValue();
            return TemplateScopedValue::New(Integer::New(value));
        }

        case st(TemplateScopedValue)::Type::Double:
        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }

    Trigger(IllegalArgumentException);
}

//operator:<<
TemplateScopedValue _TemplateFunctionItem::computeLeftShift(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);

    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            auto value = arg1->getIntValue() << arg2->getIntValue();
            return TemplateScopedValue::New(Integer::New(value));
        }

        case st(TemplateScopedValue)::Type::Double:
        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }

    Trigger(IllegalArgumentException);
}

//operator:>>
TemplateScopedValue _TemplateFunctionItem::computeRightShift(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);

    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            auto value = arg1->getIntValue() >> arg2->getIntValue();
            return TemplateScopedValue::New(Integer::New(value));
        }

        case st(TemplateScopedValue)::Type::Double:
        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }

    Trigger(IllegalArgumentException);
}

//operator:|
TemplateScopedValue _TemplateFunctionItem::computeOrOperation(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);

    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            auto value = (arg1->getIntValue() | arg2->getIntValue());
            return TemplateScopedValue::New(Integer::New(value));
        }

        case st(TemplateScopedValue)::Type::Double:
        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }

    Trigger(IllegalArgumentException);
}

//operator:&
TemplateScopedValue _TemplateFunctionItem::computeAndOperation(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);

    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            auto value = (arg1->getIntValue() & arg2->getIntValue());
            return TemplateScopedValue::New(Integer::New(value));
        }

        case st(TemplateScopedValue)::Type::Double:
        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }

    Trigger(IllegalArgumentException);
}

//operator:>
TemplateScopedValue _TemplateFunctionItem::computeMoreThan(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);
    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            return TemplateScopedValue::New(Boolean::New(arg1->getIntValue() > arg2->getIntValue()));
        }

        case st(TemplateScopedValue)::Type::Double: {
            auto arg1_double_value = arg1->getDoubleValue();
            auto arg2_double_value = arg2->getDoubleValue();
            return TemplateScopedValue::New(Boolean::New(st(Double)::Compare(arg1_double_value,arg2_double_value) > 0));
        }
        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }
    Trigger(IllegalArgumentException);
}

//operator:<
TemplateScopedValue _TemplateFunctionItem::computeLessThan(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);
    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            return TemplateScopedValue::New(Boolean::New(arg1->getIntValue() < arg2->getIntValue()));
        }

        case st(TemplateScopedValue)::Type::Double: {
            auto arg1_double_value = arg1->getDoubleValue();
            auto arg2_double_value = arg2->getDoubleValue();
            return TemplateScopedValue::New(Boolean::New(st(Double)::Compare(arg1_double_value,arg2_double_value) < 0));
        }
        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }
    Trigger(IllegalArgumentException);
}

//operator:<=
TemplateScopedValue _TemplateFunctionItem::computeLessThanOrEqual(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);
    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            return TemplateScopedValue::New(Boolean::New(arg1->getIntValue() <= arg2->getIntValue()));
        }

        case st(TemplateScopedValue)::Type::Double: {
            auto arg1_double_value = arg1->getDoubleValue();
            auto arg2_double_value = arg2->getDoubleValue();
            return TemplateScopedValue::New(Boolean::New(st(Double)::Compare(arg1_double_value,arg2_double_value) <= 0));
        }
        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }
    Trigger(IllegalArgumentException);
}

//operatpr:>=
TemplateScopedValue _TemplateFunctionItem::computeMoreThanOrEqual(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);
    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Integer: {
            return TemplateScopedValue::New(Boolean::New(arg1->getIntValue() >= arg2->getIntValue()));
        }

        case st(TemplateScopedValue)::Type::Double: {
            auto arg1_double_value = arg1->getDoubleValue();
            auto arg2_double_value = arg2->getDoubleValue();
            return TemplateScopedValue::New(Boolean::New(st(Double)::Compare(arg1_double_value,arg2_double_value) >= 0));
        }
        case st(TemplateScopedValue)::Type::String:
        case st(TemplateScopedValue)::Type::Bool:
        break;
    }
    Trigger(IllegalArgumentException);
}

//operator:&&
TemplateScopedValue _TemplateFunctionItem::computeAndCondition(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);
    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Bool: {
            return TemplateScopedValue::New(Boolean::New(arg1->getBoolValue() && arg2->getBoolValue()));
        } break;

        case st(TemplateScopedValue)::Type::Integer:
        case st(TemplateScopedValue)::Type::Double:
        case st(TemplateScopedValue)::Type::String:
        break;
    }
    Trigger(IllegalArgumentException);
}

//operator:||
TemplateScopedValue _TemplateFunctionItem::computeOrCondition(TemplateScopedValue arg1,TemplateScopedValue arg2) {
    Panic(arg1->getType() != arg2->getType(),IllegalArgumentException);
    switch(arg1->getType()) {
        case st(TemplateScopedValue)::Type::Bool: {
            return TemplateScopedValue::New(Boolean::New(arg1->getBoolValue() || arg2->getBoolValue()));
        } break;

        case st(TemplateScopedValue)::Type::Integer:
        case st(TemplateScopedValue)::Type::Double:
        case st(TemplateScopedValue)::Type::String:
        break;
    }
    Trigger(IllegalArgumentException);
}

//operator:!
TemplateScopedValue _TemplateFunctionItem::computeExclamation(TemplateScopedValue arg1) {
    Panic(arg1->getType() != st(TemplateScopedValue)::Bool,IllegalArgumentException);
    return TemplateScopedValue::New(Boolean::New(!arg1->getBoolValue()));
}

TemplateScopedValue _TemplateFunctionItem::computeAssignment(TemplateScopedValueContainer container,Object obj,
                                            String unitName,TemplateScopedValue arg2) {
    if(unitName != nullptr) {
        printf("computeAssignment unitName is [%s] \n",unitName->toChars());
        if(unitName->startsWith(".")) {
            auto field = obj->getField(unitName->subString(1,unitName->size() - 1));
            if(field != nullptr) {
                switch(field->getType()) {
                    case st(Field)::Type::Long: {
                        field->setValue(arg2->getIntValue());
                    } break;

                    case st(Field)::Type::Int: {
                        field->setValue(arg2->getIntValue());
                    } break;

                    case st(Field)::Type::Byte: {
                        field->setValue((byte)(arg2->getIntValue()));
                    }

                    case st(Field)::Type::Bool: {
                        field->setValue(arg2->getBoolValue());
                    } break;

                    case st(Field)::Type::Double: {
                        field->setValue(arg2->getDoubleValue());
                    } break;

                    case st(Field)::Type::Float: {
                        field->setValue(arg2->getDoubleValue());
                    } break;

                    case st(Field)::Type::String: {
                        field->setValue(arg2->getStringValue());
                    } break;

                    case st(Field)::Type::Uint16: {
                        field->setValue(arg2->getIntValue());
                    } break;

                    case st(Field)::Type::Uint32: {
                        field->setValue(arg2->getIntValue());
                    } break;

                    case st(Field)::Type::Uint64: {
                        field->setValue(arg2->getIntValue());
                    } break;
                }
                return nullptr;
            }
        } 
        
        auto scoped_value = container->getScopedValue(unitName);
        Panic(arg2->getType() != scoped_value->getType(),IllegalArgumentException);

        switch(arg2->getType()) {
            case st(TemplateScopedValue)::Type::Bool: {
                scoped_value->updateBoolValue(arg2->getBoolValue());
            } break;

            case st(TemplateScopedValue)::Type::Integer: {
                scoped_value->updateIntValue(arg2->getIntValue());
            } break;

            case st(TemplateScopedValue)::Type::Double: {
                scoped_value->updateDoubleValue(arg2->getDoubleValue());
            } break;

            case st(TemplateScopedValue)::Type::String: {
                scoped_value->updateStringValue(arg2->getStringValue());
            } break;
        }
        
    }

    return nullptr;
}

}
