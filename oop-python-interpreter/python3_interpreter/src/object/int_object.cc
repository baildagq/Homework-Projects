#include "int_object.h"
#include "all_object.h"
IntTypeObject *IntTypeObject::Singleton(){
    static auto singleton=new IntTypeObject();
    return singleton;
}
IntTypeObject::IntTypeObject():TypeObject("int"){
    this->tp_new=new TernaryFuncPtrWrapper([](Object *type,Object *args,Object *kwargs)->Object*{
        return new IntObject(0);
    });
    this->tp_init=new TernaryFuncPtrWrapper([](Object *obj,Object *args,Object *kwargs)->Object*{
        auto i=dynamic_cast<IntObject*>(obj);
        auto tuple=dynamic_cast<TupleObject*>(args);
        auto dict=dynamic_cast<DictObject*>(kwargs);
        assert(i!=nullptr);
        assert(tuple!=nullptr);
        assert(dict!=nullptr);
        assert(tuple->GetSize()==1);
        assert(dict->IsEmpty());
        i->v=tuple->Get(0)->ToInt();
        return NoneObject::Singleton();
    });
    this->tp_getattr=GetGeneralGetAttr();
    this->tp_hash=new UnaryFuncPtrWrapper([](Object *obj)->Object*{
        auto i=dynamic_cast<IntObject*>(obj);
        if (i!=nullptr)
            return new IntObject(i->v);
        return nullptr;
    });
    this->nb_negative=new UnaryFuncPtrWrapper([](Object *oa)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        if (a!=nullptr)
            return new IntObject(-a->v);
        return nullptr;
    });
    this->nb_positive=new UnaryFuncPtrWrapper([](Object *oa)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        if (a!=nullptr)
            return new IntObject(+a->v);
        return nullptr;
    });

    this->nb_add=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return new IntObject(a->v+b->v);
        }
        return nullptr;
    });
    this->nb_subtract=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return new IntObject(a->v-b->v);
        }
        return nullptr;
    });
    this->nb_multiply=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return new IntObject(a->v*b->v);
        }
        return nullptr;
    });
    this->nb_floor_div=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return new IntObject(a->v/b->v);
        }
        return nullptr;
    });
    this->nb_module=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return new IntObject(a->v%b->v);
        }
        return nullptr;
    });
    this->nb_lshift=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return new IntObject(a->v<<b->v);
        }
        return nullptr;
    });
    this->nb_rshift=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return new IntObject(a->v>>b->v);
        }
        return nullptr;
    });
    this->nb_bit_or=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return new IntObject(a->v|b->v);
        }
        return nullptr;
    });
    this->nb_bit_and=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return new IntObject(a->v&b->v);
        }
        return nullptr;
    });
    this->nb_bit_xor=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return new IntObject(a->v^b->v);
        }
        return nullptr;
    });

    this->nb_inplace_add=new BinaryFuncPtrWrapper(this->nb_add);
    this->nb_inplace_subtract=new BinaryFuncPtrWrapper(this->nb_subtract);
    this->nb_inplace_multiply=new BinaryFuncPtrWrapper(this->nb_multiply);
    this->nb_inplace_floor_div=new BinaryFuncPtrWrapper(this->nb_floor_div);
    this->nb_inplace_module=new BinaryFuncPtrWrapper(this->nb_module);
    this->nb_inplace_lshift=new BinaryFuncPtrWrapper(this->nb_lshift);
    this->nb_inplace_rshift=new BinaryFuncPtrWrapper(this->nb_rshift);
    this->nb_inplace_bit_or=new BinaryFuncPtrWrapper(this->nb_bit_or);
    this->nb_inplace_bit_and=new BinaryFuncPtrWrapper(this->nb_bit_and);
    this->nb_inplace_bit_xor=new BinaryFuncPtrWrapper(this->nb_bit_xor);

    this->cmp_eq=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return BoolObject::Singleton(a->v==b->v);
        }
        return nullptr;
    });
    this->cmp_ne=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return BoolObject::Singleton(a->v!=b->v);
        }
        return nullptr;
    });
    this->cmp_lt=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return BoolObject::Singleton(a->v<b->v);
        }
        return nullptr;
    });
    this->cmp_le=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return BoolObject::Singleton(a->v<=b->v);
        }
        return nullptr;
    });
    this->cmp_gt=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return BoolObject::Singleton(a->v>b->v);
        }
        return nullptr;
    });
    this->cmp_ge=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<IntObject*>(oa);
        auto b=dynamic_cast<IntObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return BoolObject::Singleton(a->v>=b->v);
        }
        return nullptr;
    });
    this->BuiltinInit();
}
IntObject::IntObject(int _v):Object(IntTypeObject::Singleton()),v(_v){}
IntObject::IntObject(const std::string &str):IntObject(std::stoi(str)){};
