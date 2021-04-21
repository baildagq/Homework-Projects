#pragma once
#include "object.h"
#include "bool_object.h"
class StringTypeObject:public TypeObject{
    StringTypeObject();
public:
    static StringTypeObject *Singleton();
};
class StringObject:public Object{
    friend StringTypeObject;
    std::string str;
public:
    std::string ToString() override;
    std::string ToRepr() override;
    std::string Get(int x);
    int GetSize() override;
    StringObject(const std::string &_str,bool from_text=false);
};
class StringIterTypeObject:public TypeObject{
    StringIterTypeObject();
public:
    static StringIterTypeObject *Singleton();
};
class StringIterObject:public Object{
    friend StringIterTypeObject;
    StringObject *string_obj;
    int x=0;
public:
    StringIterObject(StringObject *_tuple_obj);
    std::string ToString() override{return "str iter";}
};
