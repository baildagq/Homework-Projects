#include "bool_object.h"
#include "all_object.h"
BoolTypeObject *BoolTypeObject::Singleton(){
    static auto singleton=new BoolTypeObject();
    return singleton;
}
BoolTypeObject::BoolTypeObject():TypeObject("bool"){
    this->tp_new=new TernaryFuncPtrWrapper([](Object *type,Object *args,Object *kwargs)->Object*{
        auto tuple=dynamic_cast<TupleObject*>(args);
        assert(tuple!=nullptr);
        return BoolObject::Singleton(tuple->Get(0)->ToBool());
    });
    this->tp_hash=new UnaryFuncPtrWrapper([](Object *obj)->Object*{
        auto i=dynamic_cast<BoolObject*>(obj);
        if (i!=nullptr)
            return new IntObject(int(i->ToInt()));
        return nullptr;
    });
    this->tp_getattr=GetGeneralGetAttr();

}
BoolObject *BoolObject::Singleton(bool flag){
    if (flag){
        static auto singleton=new BoolObject(flag);
        return singleton;
    } else{
        static auto singleton=new BoolObject(flag);
        return singleton;
    }
}
BoolObject::BoolObject(bool _flag):Object(BoolTypeObject::Singleton()),flag(_flag){}
