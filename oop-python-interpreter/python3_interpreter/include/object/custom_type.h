#pragma once
#include "object.h"
class CustomTypeObject:public TypeObject{
public:
    CustomTypeObject(const std::string &name);
    CustomTypeObject(const std::string &name,std::vector<TypeObject*> bases);
};
class CustomObject:public Object{
public:
    CustomObject(CustomTypeObject *type);
    std::string ToString() override{return "custom";}
};
