#include "none_object.h"
NoneTypeObject *NoneTypeObject::Singleton(){
    static auto singleton=new NoneTypeObject();
    return singleton;
}
NoneTypeObject::NoneTypeObject():TypeObject("NoneType"){}
NoneObject *NoneObject::Singleton(){
    static auto singleton=new NoneObject();
    return singleton;
}
NoneObject::NoneObject():Object(NoneTypeObject::Singleton()){}
