#include "object_factory.h"
ObjectFactory *ObjectFactory::Singleton(){
    static ObjectFactory singleton;
    return &singleton;
}
Object *ObjectFactory::Handle(Object *obj){
    Singleton()->vec.push_back(obj);
    return obj;
}
ObjectFactory::~ObjectFactory(){
    for (auto obj:this->vec){
        //printf("delete obj %s\n",obj->ToRepr().c_str());
        delete obj;
    }
}

