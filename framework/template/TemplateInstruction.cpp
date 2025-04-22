#include <mutex>

#include "TemplateInstruction.hpp"

using namespace obotcha;

namespace gagira {

const String _TemplateInstruction::StartTag = String::New("{{");
const String _TemplateInstruction::EndTag = String::New("}}");
const String _TemplateInstruction::StartComment = String::New("/*");
const String _TemplateInstruction::EndComment = String::New("*/");
const String _TemplateInstruction::LineComment = String::New("//");


const String _TemplateInstruction::LetCommand = String::New("let");
const String _TemplateInstruction::LoopCommand = String::New("loop");
//const String _TemplateInstruction::RofCommand = String::New("rof");
const String _TemplateInstruction::IndexCommand = String::New("index");
const String _TemplateInstruction::InCommand = String::New("in");
//const String _TemplateInstruction::NiCommand = String::New("ni");
const String _TemplateInstruction::DotCommand = String::New(".");
const String _TemplateInstruction::IfCommand = String::New("if");
const String _TemplateInstruction::ElseIfCommand = String::New("elif");
const String _TemplateInstruction::ElseCommand = String::New("else");
//const String _TemplateInstruction::FiCommand = String::New("fi");
const String _TemplateInstruction::EndCommand = String::New("end");

const String _TemplateInstruction::IncludeCommand = String::New("template");
const String _TemplateInstruction::DefineCommand = String::New("define");
const String _TemplateInstruction::LenCommand = String::New("len");
const String _TemplateInstruction::PrintCommand = String::New("print");
const String _TemplateInstruction::WriteCommand = String::New("write");

const String _TemplateInstruction::ReturnCommand = String::New("return");
const String _TemplateInstruction::BreakCommand = String::New("break");

const String _TemplateInstruction::EqualOperator = String::New("=");

const String _TemplateInstruction::SpaceTextSymbol = String::New(" ");
const String _TemplateInstruction::QuotationTextSymbol = String::New("\"");

const String _TemplateInstruction::IntVariableType = String::New("int");
const String _TemplateInstruction::DoubleVariableType = String::New("double");
const String _TemplateInstruction::StringVariableType = String::New("string");
const String _TemplateInstruction::BoolVariableType = String::New("bool");
const String _TemplateInstruction::TrueStatement = String::New(Boolean::New(true));
const String _TemplateInstruction::FalseStatement = String::New(Boolean::New(false));

const String _TemplateInstruction::WithinMark = String::New(".");
const String _TemplateInstruction::ModularMark = String::New("%");
const String _TemplateInstruction::AndMark = String::New("&&");
const String _TemplateInstruction::OrMark = String::New("||");
const String _TemplateInstruction::OrOperationMark = String::New("|");
const String _TemplateInstruction::AndOperationMark = String::New("&");
const String _TemplateInstruction::LeftBraceMark = String::New("(");
const String _TemplateInstruction::RightBraceMark = String::New(")");
const String _TemplateInstruction::MoreThanMark = String::New(">");
const String _TemplateInstruction::MoreThanOrEqualMark = String::New(">=");
const String _TemplateInstruction::LessThanMark = String::New("<");
const String _TemplateInstruction::LessThanOrEqualMark = String::New("<=");
const String _TemplateInstruction::EqualMark = String::New("=");
const String _TemplateInstruction::NotEqualMark = String::New("!=");
const String _TemplateInstruction::ExclamationMark = String::New("!");
const String _TemplateInstruction::PlusMark = String::New("+");
const String _TemplateInstruction::PositiveMark = String::New("#+");

const String _TemplateInstruction::MinusMark = String::New("-");
const String _TemplateInstruction::NegativeMark = String::New("#-");

const String _TemplateInstruction::MultipleMark = String::New("*");
const String _TemplateInstruction::DivideMark = String::New("/");
const String _TemplateInstruction::PlusPlusMark = String::New("++");
const String _TemplateInstruction::MinusMinusMark = String::New("--");
const String _TemplateInstruction::LeftSquareBracketMark = String::New("[");
const String _TemplateInstruction::RightSquareBracketMark = String::New("]");
const String _TemplateInstruction::TildeMark = String::New("~");
const String _TemplateInstruction::LeftShiftMark = String::New("<<");
const String _TemplateInstruction::RightShiftMark = String::New(">>");
const String _TemplateInstruction::ExclusiveMark = String::New("^");



}

