#include "ref_object.h"
RefTypeObject *RefTypeObject::Singleton(){
    static auto singleton=new RefTypeObject();
    return singleton;
}
RefTypeObject::RefTypeObject():TypeObject("RefType"){
}
RefObject::RefObject(Object *_ref_obj):Object(RefTypeObject::Singleton()),ref_obj(_ref_obj){}
void RefObject::SetRef(Object *obj){
    this->ref_obj=obj;
}
Object *RefObject::GetRef(){
    return this->ref_obj;
}
