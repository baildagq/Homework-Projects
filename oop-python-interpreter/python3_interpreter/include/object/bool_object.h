#pragma once
#include "object.h"
class BoolTypeObject:public TypeObject{
    BoolTypeObject();
public:
    static BoolTypeObject *Singleton();
};
class BoolObject:public Object{
    bool flag;
    BoolObject(bool _flag);
public:
    static BoolObject *Singleton(bool flag);
    std::string ToString() override{return this->flag?"True":"False";}
    int ToInt() override{return this->flag?1:0;}
    bool ToBool() override{return this->flag;}
};
