#pragma once
#include "object.h"
class TypeTypeObject:public TypeObject{
    TypeTypeObject();
public:
    static TypeTypeObject *Singleton(bool init=true);
};
bool IsSubType(TypeObject *a,TypeObject *b);
bool IsInstance(Object *a,TypeObject *b);
