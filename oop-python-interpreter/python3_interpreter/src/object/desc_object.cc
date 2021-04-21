#include "desc_object.h"
#include "tuple_object.h"
DescTypeObject *DescTypeObject::Singleton(){
    static auto singleton=new DescTypeObject();
    return singleton;
}
DescTypeObject::DescTypeObject():TypeObject("desc"){
    this->tp_call=new TernaryFuncPtrWrapper([](Object *obj,Object *args,Object *kw)->Object*{
        auto p=dynamic_cast<DescObject*>(obj);
        assert(p!=nullptr);
        auto tuple=dynamic_cast<TupleObject*>(args);
        assert(tuple!=nullptr);
        return p->call(tuple->ToVector());
    });
}
DescObject::DescObject():Object(DescTypeObject::Singleton()){}
BinaryDescObject::BinaryDescObject(BinaryFunc *_func):func(_func){}
Object *BinaryDescObject::call(const std::vector<Object*> &vec){
    assert((int)vec.size()==2);
    return (*this->func)(vec[0],vec[1]);
}
TernaryDescObject::TernaryDescObject(TernaryFunc *_func):func(_func){}
Object *TernaryDescObject::call(const std::vector<Object*> &vec){
    assert((int)vec.size()==3);
    return (*this->func)(vec[0],vec[1],vec[2]);
}
