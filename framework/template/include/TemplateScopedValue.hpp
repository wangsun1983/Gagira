#ifndef __GAGRIA_TEMPLATE_SCOPED_VALUE_HPP__
#define __GAGRIA_TEMPLATE_SCOPED_VALUE_HPP__

#include "String.hpp"
#include "Integer.hpp"
#include "Double.hpp"

using namespace obotcha;

namespace gagira {

DECLARE_CLASS(TemplateScopedValue) {
public:
   enum Type {
       Integer = 0,
       Double,
       String,
       Bool,
   };

   _TemplateScopedValue(obotcha::String value);
   _TemplateScopedValue(obotcha::Integer value);
   _TemplateScopedValue(obotcha::Double value);
   _TemplateScopedValue(obotcha::Boolean value);

   void updateIntValue(int);
   void updateDoubleValue(double);
   void updateStringValue(obotcha::String);
   void updateBoolValue(bool);
   void updateType(int);

   void setDirectReturn(bool);
   bool isDirectReturn();

   void setDirectBreak(bool);
   bool isDirectBreak();

   int getType();
   int getIntValue();
   double getDoubleValue();
   bool getBoolValue();
   obotcha::String getStringValue();
   
   obotcha::String toString();
    
private:
    int mIntValue;
    double mDoubleValue;
    obotcha::String mStringValue;
    bool mBoolValue;
    int mType;

    bool isReturn;
    bool isBreak;

};

}

#endif
