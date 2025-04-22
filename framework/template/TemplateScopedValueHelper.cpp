#include "TemplateScopedValueHelper.hpp"
#include "Field.hpp"

namespace gagira {

MethodField _TemplateScopedValueHelper::GetMethod(String tag,TemplateObjectContainer objContainer) {    
    FetchRet(domainName,valueName,type) = disassembleParam(tag);
    if(domainName != nullptr) {
        auto obj = objContainer->getObjInLifeCycle(domainName);
        return obj->getMethodField(valueName->toChars());
    } else {
        auto obj = objContainer->getCurrent();
        return obj->getMethodField(valueName->toChars());
    }

    return nullptr;
}

TemplateScopedValue _TemplateScopedValueHelper::Get(String tag,
                                                            TemplateScopedValueContainer scopdevalueContainer,
                                                            TemplateObjectContainer objContainer) {
    FetchRet(domainName,valueName,type) = disassembleParam(tag);
    //search value like x.value
    if(domainName != nullptr) {
        auto obj = objContainer->getObjInLifeCycle(domainName);
        return SeekInObject(valueName,obj);
    } else {
        //search in objContainer first
        switch(type) {
            case Scoped:
                return scopdevalueContainer->getScopedValue(valueName);
            break;

            case Obj:
                auto obj = objContainer->getCurrent();
                return SeekInObject(valueName,obj);
            break;
        }
    }
    return nullptr;
}

void _TemplateScopedValueHelper::UpdateInObject(String fieldname,String value,Object obj) {
    auto field = obj->getField(fieldname);
    if(field != nullptr) {
        switch(field->getType()) {
            case st(Field)::Type::Long: {
                field->setValue(value->toBasicLong());
            } break;

            case st(Field)::Type::Int: {
                field->setValue(value->toBasicInt());
            } break;

            case st(Field)::Type::Byte: {
                field->setValue(value->toBasicByte());
            } break;

            case st(Field)::Type::Bool: {
                field->setValue(value->toBasicBool());
            } break;

            case st(Field)::Type::Double: {
                field->setValue(value->toBasicDouble());
            } break;

            case st(Field)::Type::Float: {
                field->setValue(value->toBasicFloat());
            } break;

            case st(Field)::Type::String: {
                field->setValue(value);
            } break;

            case st(Field)::Type::Uint16: {
                field->setValue(value->toBasicUint16());
            } break;

            case st(Field)::Type::Uint32: {
                field->setValue(value->toBasicUint32());
            } break;

            case st(Field)::Type::Uint64: {
                field->setValue(value->toBasicUint64());
            } break;
        }
    }
}

TemplateScopedValue _TemplateScopedValueHelper::SeekInObject(String tag,Object obj) {
    if(obj != nullptr) {
        try {
            auto field = obj->getField(tag);
            if(field != nullptr) {
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
    return nullptr;
}


void _TemplateScopedValueHelper::Set(String tag,String value,TemplateScopedValueContainer scopedContainer,TemplateObjectContainer objContainer) {
    FetchRet(domainName,valueName,type) = disassembleParam(tag);
    if(domainName != nullptr) {
        auto obj = objContainer->getObjInLifeCycle(domainName);
        UpdateInObject(valueName,value,obj);
    } else {
        switch(type) {
            case Scoped: {
                auto scopedValue = scopedContainer->getScopedValue(valueName);
                switch(scopedValue->getType()) {
                    case st(TemplateScopedValue)::Integer:
                        printf("_TemplateScopedValueHelper, Set Scopedvalue is %d \n",value->toBasicInt());
                        scopedValue->updateIntValue(value->toBasicInt());
                    break;

                    case st(TemplateScopedValue)::Double:
                        scopedValue->updateDoubleValue(value->toBasicDouble());
                    break;

                    case st(TemplateScopedValue)::String:
                        scopedValue->updateStringValue(value);
                    break;

                    case st(TemplateScopedValue)::Bool:
                        scopedValue->updateBoolValue(value->toBasicBool());
                    break;
                }
            } break;

            case Obj: {
                auto obj = objContainer->getCurrent();
                return UpdateInObject(valueName,value,obj);
            } break;
        }
    }
}

DefRet(String,String,int) _TemplateScopedValueHelper::disassembleParam(String tag) {
    String domainName = nullptr;
    String valueName = nullptr;
    int type = Scoped;
    int pos = tag->indexOf(".");
    if(pos == 0) {
        valueName = tag->subString(1,tag->size() - 1);
        type = Obj;
    } else if(pos == -1) {
        valueName = tag;
    } else if (pos > 0) {
        domainName = tag->subString(0,pos);
        valueName = tag->subString(pos + 1,tag->size() - pos - 1);
    }
    return MakeRet(domainName,valueName,type);
}

}

