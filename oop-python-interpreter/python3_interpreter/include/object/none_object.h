#pragma once
#include "object.h"
class NoneTypeObject:public TypeObject{
    NoneTypeObject();
public:
    static NoneTypeObject *Singleton();
};
class NoneObject:public Object{
    NoneObject();
public:
    static NoneObject *Singleton();
    std::string ToString(){return "None";}
};
