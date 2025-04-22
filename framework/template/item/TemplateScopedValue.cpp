#include "TemplateScopedValue.hpp"

using namespace obotcha;

namespace gagira {

_TemplateScopedValue::_TemplateScopedValue(obotcha::String value) {
    mStringValue = value;
    mType = Type::String;
    isReturn = false;
    isBreak = false;
}

_TemplateScopedValue::_TemplateScopedValue(obotcha::Integer value) {
    mIntValue = value->toValue();
    mType = Type::Integer;
    isReturn = false;
    isBreak = false;
}

_TemplateScopedValue::_TemplateScopedValue(obotcha::Double value) {
    mDoubleValue = value->toValue();
    mType = Type::Double;
    isReturn = false;
    isBreak = false;
}

_TemplateScopedValue::_TemplateScopedValue(obotcha::Boolean value) {
    mBoolValue = value->toValue();
    mType = Type::Bool;
    isReturn = false;
    isBreak = false;
}

void _TemplateScopedValue::setDirectReturn(bool v) {
    isReturn = v;
}

bool _TemplateScopedValue::isDirectReturn() {
    return isReturn;
}

void _TemplateScopedValue::setDirectBreak(bool v) {
    isBreak = v;
}

bool _TemplateScopedValue::isDirectBreak() {
    return isBreak;
}

void _TemplateScopedValue::updateType(int v) {
    mType = v;
}

int _TemplateScopedValue::getType() {
    return mType;
}

int _TemplateScopedValue::getIntValue() {
    return mIntValue;
}

double _TemplateScopedValue::getDoubleValue() {
    return mDoubleValue;
}

obotcha::String _TemplateScopedValue::getStringValue() {
    return mStringValue;
}

bool _TemplateScopedValue::getBoolValue() {
    return mBoolValue;
}

void _TemplateScopedValue::updateIntValue(int v) {
    mIntValue = v;
}

void _TemplateScopedValue::updateDoubleValue(double v) {
    mDoubleValue = v;
}

void _TemplateScopedValue::updateStringValue(obotcha::String v) {
    mStringValue = v;
}

void _TemplateScopedValue::updateBoolValue(bool v) {
    mBoolValue = v;
}

obotcha::String _TemplateScopedValue::toString() {
    switch(mType) {
        case Type::Double:{
            return String::New(mDoubleValue);
        }

        case Type::String:{
            return mStringValue;
        }

        case Type::Bool:{
            return String::New(mBoolValue);
        }

        case Type::Integer:{
            return String::New(mIntValue);
        }
    }

    return nullptr;
}

}
