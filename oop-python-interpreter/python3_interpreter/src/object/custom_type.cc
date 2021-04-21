#include "custom_type.h"
#include "all_object.h"
CustomTypeObject::CustomTypeObject(const std::string &name):CustomTypeObject(name,{ObjectTypeObject::Singleton()}){
}
CustomTypeObject::CustomTypeObject(const std::string &name,std::vector<TypeObject*> bases):TypeObject(name,bases){
    this->tp_new=new TernaryFuncPtrWrapper([](Object *type,Object *args,Object *kwargs)->Object*{
        auto t=dynamic_cast<CustomTypeObject*>(type);
        assert(t!=nullptr);
        return new CustomObject(t);
    });
    this->tp_getattr=GetGeneralGetAttr();
    this->tp_setattr=GetGeneralSetAttr();
    this->BuiltinInit();
}
CustomObject::CustomObject(CustomTypeObject *type):Object(type){}
