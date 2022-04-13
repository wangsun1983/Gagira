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
    printf("HtmlTemplateObjectItem start \n");
    if(fieldNames->size() == 0) {
        return _objectToString(o);
    }
    printf("HtmlTemplateObjectItem trace1,fieldNames size is %d \n",fieldNames->size());
    auto iterator = fieldNames->getIterator();
    while(iterator->hasValue()) {
        String name = iterator->getValue();
        printf("HtmlTemplateObjectItem trace1,name is %s \n",name->toChars());
        field = o->getField(name);
        if(field == nullptr) {
            break;
        }

        if(field->getType() == st(Field)::FieldTypeObject) {
            o = field->getObjectValue();
        }
        iterator->next();
    }
    printf("HtmlTemplateObjectItem trace2 \n");
    
    if(field != nullptr) {
        printf("HtmlTemplateObjectItem trace3,field type is %d \n",field->getType());
        switch(field->getType()) {
            case st(Field)::FieldTypeLong: {
                return createString(field->getLongValue());
            } break;

            case st(Field)::FieldTypeInt: {
                return createString(field->getIntValue());
            } break;

            case st(Field)::FieldTypeByte: {
                return createString(field->getByteValue());
            } break;

            case st(Field)::FieldTypeBool: {
                return createString(field->getBoolValue());
            } break;

            case st(Field)::FieldTypeDouble: {
                return createString(field->getDoubleValue());
            } break;

            case st(Field)::FieldTypeFloat: {
                return createString(field->getFloatValue());
            } break;

            case st(Field)::FieldTypeString: {
                printf("fild string value is %s \n",field->getStringValue()->toChars());
                return createString(field->getStringValue());
            } break;

            case st(Field)::FieldTypeUint16: {
                return createString(field->getUint16Value());
            } break;

            case st(Field)::FieldTypeUint32: {
                return createString(field->getUint32Value());
            } break;

            case st(Field)::FieldTypeUint64: {
                return createString(field->getUint64Value());
            } break;

            case st(Field)::FieldTypeObject: {
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
