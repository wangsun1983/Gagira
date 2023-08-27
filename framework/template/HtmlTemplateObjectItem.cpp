#include "HtmlTemplateObjectItem.hpp"
#include "Field.hpp"
#include "OStdInstanceOf.hpp"

using namespace obotcha;

namespace gagira {

_HtmlTemplateObjectItem::_HtmlTemplateObjectItem() {
    fieldNames = createArrayList<String>();
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
                return createString(field->getLongValue());
            } break;

            case st(Field)::Type::Int: {
                return createString(field->getIntValue());
            } break;

            case st(Field)::Type::Byte: {
                return createString(field->getByteValue());
            } break;

            case st(Field)::Type::Bool: {
                return createString(field->getBoolValue());
            } break;

            case st(Field)::Type::Double: {
                return createString(field->getDoubleValue());
            } break;

            case st(Field)::Type::Float: {
                return createString(field->getFloatValue());
            } break;

            case st(Field)::Type::String: {
                return createString(field->getStringValue());
            } break;

            case st(Field)::Type::Uint16: {
                return createString(field->getUint16Value());
            } break;

            case st(Field)::Type::Uint32: {
                return createString(field->getUint32Value());
            } break;

            case st(Field)::Type::Uint64: {
                return createString(field->getUint64Value());
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
        return createString(Cast<Integer>(obj));
    } else if (IsInstance(Long, obj)) {
        return createString(Cast<Long>(obj));
    } else if (IsInstance(Boolean, obj)) {
        return createString(Cast<Boolean>(obj));
    } else if (IsInstance(Double, obj)) {
        return createString(Cast<Double>(obj));
    } else if (IsInstance(Float, obj)) {
        return createString(Cast<Float>(obj));
    } else if (IsInstance(Byte, obj)) {
        return createString(Cast<Byte>(obj));
    } else if (IsInstance(Uint8, obj)) {
        return createString(Cast<Uint8>(obj));
    } else if (IsInstance(Uint16, obj)) {
        return createString(Cast<Uint16>(obj));
    } else if (IsInstance(Uint32, obj)) {
        return createString(Cast<Uint32>(obj));
    } else if (IsInstance(Uint64, obj)) {
        return createString(Cast<Uint64>(obj));
    } else if (IsInstance(String, obj)) {
        return Cast<String>(obj);
    }

    return nullptr;
}

}
