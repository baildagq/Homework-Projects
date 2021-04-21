#pragma once
#include "object.h"
#include "bool_object.h"
class RangeTypeObject:public TypeObject{
    RangeTypeObject();
public:
    static RangeTypeObject *Singleton();
};
class RangeObject:public Object{
    friend RangeTypeObject;
    friend class RangeIterTypeObject;
    friend class RangeIterObject;
    int l,r,step;
public:
    RangeObject(int _l,int _r,int _step);
    RangeObject(int _l,int _r);
    RangeObject(int _r);
    std::string ToString() override{return "range";}
};
class RangeIterTypeObject:public TypeObject{
    RangeIterTypeObject();
public:
    static RangeIterTypeObject *Singleton();
};
class RangeIterObject:public Object{
    friend RangeIterTypeObject;
    RangeObject *range_obj;
    int x;
public:
    RangeIterObject(RangeObject *_range_obj);
    std::string ToString() override{return "range iter";}
};
