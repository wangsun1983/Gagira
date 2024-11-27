#include "TemplateScopedValueSeeker.hpp"
#include "Field.hpp"

namespace gagira {

TemplateScopedValue _TemplateScopedValueSeeker::Get(String tag,
                                                            TemplateScopedValueContainer scopdevalueContainer,
                                                            TemplateObjectContainer objContainer) {
    //analyse tag
    String mDomainName = nullptr;
    String mValueName = nullptr;
    int type = Scoped;

    int pos = tag->indexOf(".");
    if(pos == 0) {
        mValueName = tag->subString(1,tag->size() - 1);
        type = Obj;
    } else if(pos == -1) {
        mValueName = tag;
    } else if (pos > 0) {
        mDomainName = tag->subString(0,pos);
        mValueName = tag->subString(pos + 1,tag->size() - pos - 1);
    }
    printf("TemplateScopedValueSeeker mDomainName is %s, mValueName is %s\n",mDomainName->toChars(),mValueName->toChars());
    //search value like x.value
    if(mDomainName != nullptr) {
        auto obj = objContainer->getObjInLifeCycle(mDomainName);
        return SeekInObject(mValueName,obj);
    } else {
        //search in objContainer first
        switch(type) {
            case Scoped:
                return scopdevalueContainer->getScopedValue(mValueName);
            break;

            case Obj:
                auto obj = objContainer->getCurrent();
                return SeekInObject(mValueName,obj);
            break;
        }
    }
    return nullptr;
}

TemplateScopedValue _TemplateScopedValueSeeker::SeekInObject(String tag,Object obj) {
    printf("SeekInObject,start tag is %s \n",tag->toChars());
    if(obj != nullptr) {
        printf("SeekInObject,trace1 \n");
        try {
            auto field = obj->getField(tag);
            printf("SeekInObject,trace2 \n");
            if(field != nullptr) {
                printf("SeekInObject,trace3 \n");
                switch(field->getType()) {
                    case st(Field)::Type::Long: {
                        return TemplateScopedValue::New(Integer::New(field->getLongValue()));
                    }

                    case st(Field)::Type::Int: {
                        return TemplateScopedValue::New(Integer::New(field->getIntValue()));
                    }

                    case st(Field)::Type::Byte: {
                        return TemplateScopedValue::New(Integer::New(field->getByteValue()));
                    }

                    case st(Field)::Type::Bool: {
                        return TemplateScopedValue::New(Boolean::New(field->getBoolValue()));
                    }

                    case st(Field)::Type::Double: {
                        return TemplateScopedValue::New(Double::New(field->getDoubleValue()));
                    }

                    case st(Field)::Type::Float: {
                        return TemplateScopedValue::New(Double::New(field->getFloatValue()));
                    }

                    case st(Field)::Type::String: {
                        return TemplateScopedValue::New(field->getStringValue());
                    }

                    case st(Field)::Type::Uint16: {
                        return TemplateScopedValue::New(Integer::New(field->getUint16Value()));
                    }

                    case st(Field)::Type::Uint32: {
                        return TemplateScopedValue::New(Integer::New(field->getUint32Value()));
                    }

                    case st(Field)::Type::Uint64: {
                        return TemplateScopedValue::New(Integer::New(field->getUint64Value()));
                    }
                }
            }
        } catch(...){}
    }
    printf("SeekInObject,trace4 \n");
    return nullptr;
}

}

