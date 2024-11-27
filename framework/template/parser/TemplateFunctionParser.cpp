#include <mutex>

#include "TemplateFunctionParser.hpp"
#include "LinkedList.hpp"
#include "ForEveryOne.hpp"

namespace gagira {

HashMap<String,Integer> _TemplateFunctionParser::PriorityMaps = nullptr;
char _TemplateFunctionParser::OperatorCharArray[128] = {0};

_TemplateFunctionParser::_TemplateFunctionParser(String cmd) {
    mCmd = cmd;
    mType = Function;

    /**
    * priority          operator                                               Description
    *    1        ()   []   .                                       Function call, scope, array/member access
    *    2        !   ~   -   +  ++   --                            (most) unary operators, "-" means negative number(we use #-)
    *    3        *   /   %                                         Multiplication, division, modulo
    *    4        +   -                                             Addition and subtraction
    *    5        <<   >>                                           Bitwise shift left and right
    *    6        <   <=   >   >=                                   Comparisons: less-than and greater-than
    *    7        ==   !=                                           Comparisons: equal and not equal
    *    8        &                                                 Bitwise AND
    *    9        ^                                                 Bitwise exclusive OR (XOR)
    *    10       |                                                 Bitwise inclusive (normal) OR
    *    11       &&                                                Logical AND
    *    12       ||                                                Logical OR
    *    13       ? :                                               Conditional expression (ternary) [Do not support]
    *    14       =  +=  -=  *=  /=  %=  &=   |=   ^=   <<=   >>=   Assignment operators (right to left)
    *    15       ,                                                 Comma operator
    */
    static std::once_flag flag;
    std::call_once(flag,[] {
        //init OperatorCharArray
        memset(OperatorCharArray,0,128);
        OperatorCharArray['!'] = 1;
        OperatorCharArray['%'] = 1;
        OperatorCharArray['&'] = 1;
        OperatorCharArray['('] = 1;
        OperatorCharArray[')'] = 1;
        OperatorCharArray['*'] = 1;
        OperatorCharArray['+'] = 1;
        OperatorCharArray[','] = 1;
        OperatorCharArray['-'] = 1;
        OperatorCharArray['/'] = 1;
        OperatorCharArray['<'] = 1;
        OperatorCharArray['>'] = 1;
        OperatorCharArray['='] = 1;
        OperatorCharArray['~'] = 1;
        OperatorCharArray['|'] = 1;
        OperatorCharArray['&'] = 1;


        PriorityMaps = HashMap<String,Integer>::New();
        //1
        auto priority_1 = Integer::New(1);
        PriorityMaps->put(LeftBraceMark,priority_1);
        PriorityMaps->put(RightBraceMark,priority_1);
        PriorityMaps->put(LeftSquareBracketMark,priority_1);
        PriorityMaps->put(RightSquareBracketMark,priority_1);
        PriorityMaps->put(WithinMark,priority_1);

        //2
        auto priority_2 = Integer::New(2);
        PriorityMaps->put(ExclamationMark,priority_2);
        PriorityMaps->put(TildeMark,priority_2);
        // Negtivae/Positive/ (set as speciall case)
        PriorityMaps->put(PlusPlusMark,priority_2);
        PriorityMaps->put(MinusMinusMark,priority_2);
        PriorityMaps->put(NegativeMark,priority_2);
        PriorityMaps->put(PositiveMark,priority_2);

        //3
        auto priority_3 = Integer::New(3);
        PriorityMaps->put(MultipleMark,priority_3);
        PriorityMaps->put(DivideMark,priority_3);
        PriorityMaps->put(ModularMark,priority_3);

        //4
        auto priority_4 = Integer::New(4);
        PriorityMaps->put(MinusMark,priority_4);
        PriorityMaps->put(PlusMark,priority_4);
        //Plus/Minus (set as speciall case)

        //5
        auto priority_5 = Integer::New(5);
        PriorityMaps->put(LeftShiftMark,priority_5);
        PriorityMaps->put(RightShiftMark,priority_5);

        //6
        auto priority_6 = Integer::New(6);
        PriorityMaps->put(LessThanMark,priority_6);
        PriorityMaps->put(LessThanOrEqualMark,priority_6);
        PriorityMaps->put(MoreThanMark,priority_6);
        PriorityMaps->put(MoreThanOrEqualMark,priority_6);

        //7
        // /Comparisons: equal and not equal
        auto priority_7 = Integer::New(7);
        PriorityMaps->put(EqualMark,priority_7);
        PriorityMaps->put(NotEqualMark,priority_7);

        //8
        auto priority_8 = Integer::New(8);
        PriorityMaps->put(AndOperationMark,priority_8);

        //9
        auto priority_9 = Integer::New(9);
        PriorityMaps->put(ExclusiveMark,priority_9);

        //10
        auto priority_10 = Integer::New(10);
        PriorityMaps->put(OrOperationMark,priority_10);

        //11
        auto priority_11 = Integer::New(11);
        PriorityMaps->put(AndMark,priority_11);

        //12
        auto priority_12 = Integer::New(12);
        PriorityMaps->put(OrMark,priority_12);
    });
}

TemplateItem _TemplateFunctionParser::doParse() {

    ArrayList<TemplateFuncExpression> expressions = ArrayList<TemplateFuncExpression>::New();
    LinkedList<String> operators = LinkedList<String>::New();
    
    int cmdlen = mCmd->size();
    auto cmdchars = mCmd->toChars();
    int operator_start = -1;
    int param_start = -1;
    for(int index = 0; index < cmdlen;index++) {
        auto c = cmdchars[index];
        if(c == ' ') {
            continue;
        }

        if(isOperator(c)) {
            //find a operator,
            //check .
            if(param_start != -1) {
                auto paramString = mCmd->subString(param_start,index - param_start)->trim();
                TemplateFuncExpression expression = nullptr;

                if(c == '(') {
                    //is it a function????
                    //find next )
                    auto brace_start = index;
                    for(;index < cmdlen;index++) {
                        if(cmdchars[index] == ')') {
                            break;
                        }
                    }

                    String params_str = nullptr;
                    if(index > brace_start + 1) {
                        params_str = mCmd->subString(brace_start + 1,index - brace_start - 1)->trim();
                        auto params_data = params_str->split(",");
                        expression = TemplateFuncExpression_Function::New(paramString,params_data); //function with params
                    } else {
                        expression = TemplateFuncExpression_Function::New(paramString); //function without params
                    }
                    index++;
                    
                } else {
                    expression = createVariableExpression(paramString);
                }
                expressions->add(expression);
                param_start = -1;
            }
            

            //expression like:a+func()
            if(index >= cmdlen) {
                break;
            }

            //detect the end of the operator
            auto next = index + 1;
            if(next <= cmdlen) {
                auto next_c = cmdchars[next];
                if((c == '+' && next_c == '+')
                    ||(c == '-' && next_c == '-')
                    ||(c == '<' && next_c == '=')
                    ||(c == '<' && next_c == '<')
                    ||(c == '>' && next_c == '=')
                    ||(c == '>' && next_c == '>')
                    ||(c == '&' && next_c == '&')
                    ||(c == '|' && next_c == '|')
                    ||(c == '!' && next_c == '=')
                    ||(c == '=' && next_c == '=')) {
                    auto m_operator = mCmd->subString(index,2);
                    auto prev_operator = operators->peekLast();
                    auto prev_priority = PriorityMaps->get(prev_operator);
                    auto current_priority = PriorityMaps->get(m_operator);

                    if(prev_operator != nullptr 
                        &&!prev_operator->sameAs("(") 
                        && prev_priority->toValue() <= current_priority->toValue()) {
                        expressions->add(TemplateFuncExpression_Operator::New(operators->takeLast()));
                    }
                    operators->putLast(m_operator);
                    operator_start = -1;
                    index++;
                    continue;
                }
            }

            auto m_operator = String::New(c);
            if(operators->size() == 0) {
                m_operator = tranUnaryOperator(cmdchars,index);
                operators->putLast(m_operator);
            } else {
                //if it is ),pop all the operator
                if(m_operator->sameAs(")")) {
                    //push operator to list
                    while(!operators->isEmpty()) {
                        auto v = operators->takeLast();
                        if(v->sameAs("(")) {
                            break;
                        } else {
                            //params->add(v);
                            expressions->add(TemplateFuncExpression_Operator::New(v));
                        }
                    }
                    continue;
                }
                
                m_operator = tranUnaryOperator(cmdchars,index); 

                auto prev_operator = operators->peekLast();
                auto prev_priority = PriorityMaps->get(prev_operator);
                auto current_priority = PriorityMaps->get(m_operator);

                if(!prev_operator->sameAs("(") && prev_priority->toValue() <= current_priority->toValue()) {
                    expressions->add(TemplateFuncExpression_Operator::New(operators->takeLast()));
                }
                operators->putLast(m_operator);
            }
        } else {
            //find param
            if(param_start == -1) {
                param_start = index;
            }
        }
    }
    
    //push last item
    if(param_start != -1) {
        auto last = mCmd->subString(param_start,cmdlen - param_start);
        expressions->add(createVariableExpression(last));
    }

    while(!operators->isEmpty()) {
        auto last = operators->takeLast();
        //params->add(last);
        expressions->add(TemplateFuncExpression_Operator::New(last));
    }

    //TODO
    ForEveryOne(expr,expressions) {
        expr->dump();
    }

    auto item = TemplateFunctionItem::New(expressions);
#ifdef DEBUG_TEMPLATE_ITEM_COMMAND
    item->setCmd(mCmd);
#endif
    return item;
}

bool _TemplateFunctionParser::isOperator(char c) {
    return OperatorCharArray[(int)c] == 1;
}

String _TemplateFunctionParser::tranUnaryOperator(const char*cmd,int index) {
    auto c = cmd[index];
    if(c == '-') {
        if(isUnaryOperator(cmd,index)) {
            return NegativeMark;
        }
    } else if(c == '+') {
        if(isUnaryOperator(cmd,index)) {
            return PositiveMark;
        }
    }

    return String::New(c);
}

bool _TemplateFunctionParser::isUnaryOperator(const char *cmd,int index) {
    for(int i = index - 1;i >= 0;i--) {
        if(cmd[i] == ' ') {
            continue;
        }
        if(cmd[i] == '(') {
            return true;
        }
        return false;
    }
    return true;
}


TemplateFuncExpression _TemplateFunctionParser::createVariableExpression(String param) {
    Integer int_value = st(Integer)::Parse(param);
    if(int_value != nullptr) {
        return TemplateFuncExpression_Variable::New(TemplateScopedValue::New(int_value));
    }

    Double double_value = st(Double)::Parse(param);
    if(double_value != nullptr) {
        return TemplateFuncExpression_Variable::New(TemplateScopedValue::New(double_value));
    }

    Boolean bool_value = st(Boolean)::Parse(param);
    if(bool_value != nullptr) {
        return TemplateFuncExpression_Variable::New(TemplateScopedValue::New(bool_value));
    }

    return TemplateFuncExpression_Variable::New(TemplateScopedValue::New(param));
}




}
