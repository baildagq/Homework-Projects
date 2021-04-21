#pragma once
#include "object.h"
#include "code_object.h"
#include "tuple_object.h"
class FuncTypeObject:public TypeObject{
    FuncTypeObject();
public:
    static FuncTypeObject *Singleton();
};
class FuncObject:public Object{
    friend FuncTypeObject;
public:
    std::string name;
    CodeObject *code;
    std::map<std::string,Object*> *builtins;
    std::map<std::string,Object*> *globals;
    TupleObject *closure;
    FuncObject(const std::string &_name,CodeObject *_code,std::map<std::string,Object*> *_builtins,std::map<std::string,Object*> *_globals,TupleObject *_closure);
    std::string ToString() override{return "<function '"+this->name+"'>";}
};
