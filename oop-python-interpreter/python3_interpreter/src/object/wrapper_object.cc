#include "wrapper_object.h"
WrapperTypeObject *WrapperTypeObject::Singleton(){
    static auto singleton=new WrapperTypeObject();
    return singleton;
}
WrapperTypeObject::WrapperTypeObject():TypeObject("wrapper"){}
WrapperObject::WrapperObject(Object *_obj,Object *_attr):Object(WrapperTypeObject::Singleton()),obj(_obj),attr(_attr){
}
std::string WrapperObject::ToString(){
    return this->attr->ToString();
}
Object *WrapperObject::GetAttr(Object *key){
    return this->attr->GetAttr(key);
}
Object *WrapperObject::SetAttr(Object *key,Object *value){
    return this->attr->SetAttr(key,value);
}
