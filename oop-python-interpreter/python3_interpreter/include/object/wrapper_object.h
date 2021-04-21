#pragma once
#include "object.h"
class WrapperTypeObject:public TypeObject{
    WrapperTypeObject();
public:
    static WrapperTypeObject *Singleton();
};
class WrapperObject:public Object{
    friend WrapperTypeObject;
public:
    Object *obj,*attr;
    WrapperObject(Object *obj,Object *attr);
    std::string ToString() override;
    Object *GetAttr(Object*) override;
    Object *SetAttr(Object*,Object*) override;
};
