#pragma once
#include "object.h"
#include "code_object.h"
#include "tuple_object.h"
class GeneratorTypeObject:public TypeObject{
    GeneratorTypeObject();
public:
    static GeneratorTypeObject *Singleton();
};
class GeneratorObject:public Object{
    friend GeneratorTypeObject;
public:
    std::string name;
    CodeObject *code;
    std::map<std::string,Object*> *builtins;
    std::map<std::string,Object*> *globals;
    TupleObject *closure;
    void *eval;
    Object *(*next)(Object*);
    GeneratorObject(const std::string &_name,CodeObject *_code,std::map<std::string,Object*> *_builtins,std::map<std::string,Object*> *_globals,TupleObject *_closure);
    std::string ToString() override{return "<generator '"+this->name+"'>";}
};
