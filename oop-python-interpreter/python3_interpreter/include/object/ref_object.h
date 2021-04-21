#pragma once
#include "object.h"
class RefTypeObject:public TypeObject{
    RefTypeObject();
public:
    static RefTypeObject *Singleton();
};
class RefObject:public Object{
    Object *ref_obj;
public:
    RefObject(Object *_ref_obj);
    void SetRef(Object *obj);
    Object *GetRef();
    std::string ToString() override{return "ref";}
};
