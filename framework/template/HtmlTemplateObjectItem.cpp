#include "HtmlTemplateObjectItem.hpp"
#include "Field.hpp"
#include "OStdInstanceOf.hpp"

using namespace obotcha;

namespace gagira {

_HtmlTemplateObjectItem::_HtmlTemplateObjectItem() {
    fieldNames = ArrayList<String>::New();
}

String _HtmlTemplateObjectItem::toString(Object o) {
    Field field = nullptr;
    //it is only a {{.}}
    if(fieldNames->size() == 0) {
        return _objectToString(o);
    }
    auto iterator = fieldNames->getIterator();
    while(iterator->hasValue()) {
        String name = iterator->getValue();
        field = o->getField(name);
        if(field == nullptr) {
            break;
        }

        if(field->getType() == st(Field)::Type::Object) {
            o = field->getObjectValue();
        }
        iterator->next();
    }

    if(field != nullptr) {
        switch(field->getType()) {
            case st(Field)::Type::Long: {
                return String::New(field->getLongValue());
            } break;

            case st(Field)::Type::Int: {
                return String::New(field->getIntValue());
            } break;

            case st(Field)::Type::Byte: {
                return String::New(field->getByteValue());
            } break;

            case st(Field)::Type::Bool: {
                return String::New(field->getBoolValue());
            } break;

            case st(Field)::Type::Double: {
                return String::New(field->getDoubleValue());
            } break;

            case st(Field)::Type::Float: {
                return String::New(field->getFloatValue());
            } break;

            case st(Field)::Type::String: {
                return String::New(field->getStringValue());
            } break;

            case st(Field)::Type::Uint16: {
                return String::New(field->getUint16Value());
            } break;

            case st(Field)::Type::Uint32: {
                return String::New(field->getUint32Value());
            } break;

            case st(Field)::Type::Uint64: {
                return String::New(field->getUint64Value());
            } break;

            case st(Field)::Type::Object: {
                return _objectToString(field->getObjectValue());
            } break;
        }
    }

    return nullptr;
}

String _HtmlTemplateObjectItem::_objectToString(Object obj) {
    if (IsInstance(Integer, obj)) {
        return String::New(Cast<Integer>(obj));
    } else if (IsInstance(Long, obj)) {
        return String::New(Cast<Long>(obj));
    } else if (IsInstance(Boolean, obj)) {
        return String::New(Cast<Boolean>(obj));
    } else if (IsInstance(Double, obj)) {
        return String::New(Cast<Double>(obj));
    } else if (IsInstance(Float, obj)) {
        return String::New(Cast<Float>(obj));
    } else if (IsInstance(Byte, obj)) {
        return String::New(Cast<Byte>(obj));
    } else if (IsInstance(Uint8, obj)) {
        return String::New(Cast<Uint8>(obj));
    } else if (IsInstance(Uint16, obj)) {
        return String::New(Cast<Uint16>(obj));
    } else if (IsInstance(Uint32, obj)) {
        return String::New(Cast<Uint32>(obj));
    } else if (IsInstance(Uint64, obj)) {
        return String::New(Cast<Uint64>(obj));
    } else if (IsInstance(String, obj)) {
        return Cast<String>(obj);
    }

    return nullptr;
}

}
