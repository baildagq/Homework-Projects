#include "object_object.h"
#include "type_object.h"
ObjectTypeObject *ObjectTypeObject::Singleton(bool init){
    static auto singleton=new ObjectTypeObject();
    static bool first=true;
    if (init&&first){
        singleton->type_obj=TypeTypeObject::Singleton(false);
        first=false;
    }
    return singleton;
}
ObjectTypeObject::ObjectTypeObject():TypeObject(static_cast<Object*>(nullptr)){
}
