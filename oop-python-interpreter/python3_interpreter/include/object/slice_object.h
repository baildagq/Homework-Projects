#pragma once
#include "object.h"

class SliceTypeObject:public TypeObject{
    SliceTypeObject();
public:
    static SliceTypeObject *Singleton();
};

class SliceObject:public Object{
    friend SliceTypeObject;
public:
    int *lower, *upper, *step;
    SliceObject(Object* start, Object* stop, Object* len);
    std::string ToString() override;
};
     

