#pragma once
#include "object.h"
#include "bool_object.h"
class IntTypeObject:public TypeObject{
    IntTypeObject();
public:
    static IntTypeObject *Singleton();
};
class IntObject:public Object{
    friend IntTypeObject;
    int v;
public:
    IntObject(int _v);
    IntObject(const std::string &str);
    std::string ToString(){return std::to_string(this->v);}
    int ToInt(){return this->v;}
    bool ToBool(){return this->v!=0;}
};
