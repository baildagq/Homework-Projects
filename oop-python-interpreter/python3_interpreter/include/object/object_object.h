#pragma once
#include "object.h"
class ObjectTypeObject:public TypeObject{
    ObjectTypeObject();
public:
    static ObjectTypeObject *Singleton(bool init=true);
};
