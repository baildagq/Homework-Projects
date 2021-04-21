#pragma once
#include "object.h"
class BuiltinFuncTypeObject:public TypeObject{
    BuiltinFuncTypeObject();
public:
    static BuiltinFuncTypeObject *Singleton();
};
typedef Object *(*BuiltinCallFunc)(std::vector<Object*> args,std::map<std::string,Object*> kw);
class BuiltinFuncObject:public Object{
public:
    BuiltinCallFunc callfunc=nullptr;
    BuiltinFuncObject(BuiltinCallFunc callfunc);
    std::string ToString() override{return "BuiltinFunc";}
};
