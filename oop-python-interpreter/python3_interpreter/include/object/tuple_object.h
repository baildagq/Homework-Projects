#pragma once
#include "object.h"
class TupleTypeObject:public TypeObject{
    TupleTypeObject();
public:
    static TupleTypeObject *Singleton();
};
class TupleObject:public Object{
    std::vector<Object*> vec;
public:
    TupleObject(int n);
    TupleObject(const std::vector<Object*>&);
    std::string ToString() override;
    void Set(int i,Object *obj);
    Object *Get(int i);
    int GetSize() override;
    Object* Count(Object *obj);
    std::vector<Object*> ToVector();
};
class TupleIterTypeObject:public TypeObject{
    TupleIterTypeObject();
public:
    static TupleIterTypeObject *Singleton();
};
class TupleIterObject:public Object{
    friend TupleIterTypeObject;
    TupleObject *tuple_obj;
    int x=0;
public:
    TupleIterObject(TupleObject *_tuple_obj);
    std::string ToString() override{return "list iter of "+tuple_obj->ToString();}
};
