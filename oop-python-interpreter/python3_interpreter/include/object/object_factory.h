#pragma once
#include "all_object.h"
class ObjectFactory{
    std::vector<Object*> vec;
    ObjectFactory(){}
public:
    static ObjectFactory *Singleton();
    static Object *Handle(Object*);
    ~ObjectFactory();
};
