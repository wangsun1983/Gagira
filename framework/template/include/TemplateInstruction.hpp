#ifndef __GAGRIA_TEMPLATE_INSTRUCTION_HPP__
#define __GAGRIA_TEMPLATE_INSTRUCTION_HPP__

#include "String.hpp"
#include "HashMap.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateInstruction) {
public:
    static const String StartTag;
    static const String EndTag;

    static const String StartComment;
    static const String EndComment;
    static const String LineComment;

    static const String LetCommand;
    static const String LoopCommand;
    //static const String RofCommand;
    static const String IndexCommand;
    static const String InCommand;
    //static const String NiCommand;
    static const String DotCommand;
    static const String IfCommand;
    static const String ElseIfCommand;
    static const String ElseCommand;
    static const String EndCommand;
    //static const String FiCommand;
    static const String IncludeCommand;
    static const String DefineCommand;
    static const String LenCommand;
    static const String PrintCommand;
    static const String WriteCommand;

    static const String ReturnCommand;
    static const String BreakCommand;

    static const String EqualOperator;

    static const String SpaceTextSymbol;
    static const String QuotationTextSymbol;

    static const String IntVariableType;
    static const String DoubleVariableType;
    static const String StringVariableType;
    static const String BoolVariableType;

    static const String TrueStatement;
    static const String FalseStatement;

    static const String WithinMark;
    static const String ModularMark;
    static const String AndMark;
    static const String AndOperationMark;
    static const String OrMark;
    static const String OrOperationMark;
    static const String LeftBraceMark;
    static const String RightBraceMark;
    static const String MoreThanMark;
    static const String MoreThanOrEqualMark;
    static const String LessThanMark;
    static const String LessThanOrEqualMark;
    static const String EqualMark;
    static const String NotEqualMark;
    static const String ExclamationMark;
    static const String PlusMark;
    static const String PositiveMark;

    static const String MinusMark; //1.minus operation 2.negative number
    static const String NegativeMark; //1.minus operation 2.negative number

    static const String MultipleMark;
    static const String DivideMark;
    static const String PlusPlusMark;
    static const String MinusMinusMark;
    static const String LeftSquareBracketMark;
    static const String RightSquareBracketMark;
    static const String TildeMark;
    static const String LeftShiftMark;
    static const String RightShiftMark;
    static const String ExclusiveMark;

};

}

#endif
