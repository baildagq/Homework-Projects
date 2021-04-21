#include "builtin_func_object.h"
BuiltinFuncTypeObject *BuiltinFuncTypeObject::Singleton(){
    static auto singleton=new BuiltinFuncTypeObject();
    return singleton;
}
BuiltinFuncTypeObject::BuiltinFuncTypeObject():TypeObject("builtin_function"){}
BuiltinFuncObject::BuiltinFuncObject(BuiltinCallFunc _callfunc):Object(BuiltinFuncTypeObject::Singleton()),callfunc(_callfunc){}
