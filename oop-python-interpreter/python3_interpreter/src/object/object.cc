#include "object.h"
#include "all_object.h"
namespace{
    Object *BinaryOP(Object *a,Object *b,BinaryFunc *f,BinaryFunc *g){
        if (f!=nullptr&&(g==nullptr||!IsSubType(b->type_obj,a->type_obj))){
            auto res=(*f)(a,b);
            if (res!=nullptr) return res;
        }
        if (g!=nullptr){
            auto res=(*g)(b,a);
            if (res!=nullptr) return res;
        }
        return nullptr;
    }
}
Object::Object(TypeObject *_type_obj):type_obj(_type_obj){
    ObjectFactory::Handle(this);
}
Object *Object::GetAttr(Object *name){
    auto obj=this;
    auto getattr=obj->type_obj->tp_getattr;
    assert(getattr!=nullptr);
    return (*getattr)(obj,name);
}
Object *Object::SetAttr(Object *name,Object *value){
    auto obj=this;
    auto setattr=obj->type_obj->tp_setattr;
    assert(setattr!=nullptr);
    return (*setattr)(obj,name,value);
}
NumberMethod::~NumberMethod(){
    delete nb_negative;
    delete nb_positive;

    delete nb_add;
    delete nb_radd;
    delete nb_inplace_add;

    delete nb_subtract;
    delete nb_rsubtract;
    delete nb_inplace_subtract;

    delete nb_multiply;
    delete nb_rmultiply;
    delete nb_inplace_multiply;

    delete nb_floor_div;
    delete nb_rfloor_div;
    delete nb_inplace_floor_div;

    delete nb_true_div;
    delete nb_rtrue_div;
    delete nb_inplace_true_div;

    delete nb_module;
    delete nb_rmodule;
    delete nb_inplace_module;

    delete nb_lshift;
    delete nb_rlshift;
    delete nb_inplace_lshift;

    delete nb_rshift;
    delete nb_rrshift;
    delete nb_inplace_rshift;

    delete nb_bit_and;
    delete nb_rbit_and;
    delete nb_inplace_bit_and;

    delete nb_bit_or;
    delete nb_rbit_or;
    delete nb_inplace_bit_or;

    delete nb_bit_xor;
    delete nb_rbit_xor;
    delete nb_inplace_bit_xor;
}
CompareMethod::~CompareMethod(){
    delete cmp_eq;
    delete cmp_ne;
    delete cmp_lt;
    delete cmp_le;
    delete cmp_gt;
    delete cmp_ge;
}
IterMethod::~IterMethod(){
    delete iter_get_iter;
    delete iter_get_next;
}
TypeObject::~TypeObject(){
    //printf("delete type %s\n",this->ToString().c_str());
    delete tp_new;
    delete tp_init;
    delete tp_call;
    delete tp_hash;
    delete tp_getattr;
    delete tp_setattr;
    delete tp_getitem;
    delete tp_setitem;
}
Object *NumberMethod::NumberAdd(Object *a,Object *b){
    auto f=a->type_obj->nb_add;
    auto g=b->type_obj->nb_radd;
    return BinaryOP(a,b,f,g);
}
Object *NumberMethod::NumberSubtract(Object *a,Object *b){
    auto f=a->type_obj->nb_subtract;
    auto g=b->type_obj->nb_rsubtract;
    return BinaryOP(a,b,f,g);
}
Object *NumberMethod::NumberMultiply(Object *a,Object *b){
    auto f=a->type_obj->nb_multiply;
    auto g=b->type_obj->nb_rmultiply;
    return BinaryOP(a,b,f,g);
}
Object *NumberMethod::NumberFloorDiv(Object *a,Object *b){
    auto f=a->type_obj->nb_floor_div;
    auto g=b->type_obj->nb_rfloor_div;
    return BinaryOP(a,b,f,g);
}
Object *NumberMethod::NumberTrueDiv(Object *a,Object *b){
    auto f=a->type_obj->nb_true_div;
    auto g=b->type_obj->nb_rtrue_div;
    return BinaryOP(a,b,f,g);
}
Object *NumberMethod::NumberModule(Object *a,Object *b){
    auto f=a->type_obj->nb_module;
    auto g=b->type_obj->nb_rmodule;
    return BinaryOP(a,b,f,g);
}
Object *NumberMethod::NumberLShift(Object *a,Object *b){
    auto f=a->type_obj->nb_lshift;
    auto g=b->type_obj->nb_rlshift;
    return BinaryOP(a,b,f,g);
}
Object *NumberMethod::NumberRShift(Object *a,Object *b){
    auto f=a->type_obj->nb_rshift;
    auto g=b->type_obj->nb_rrshift;
    return BinaryOP(a,b,f,g);
}
Object *NumberMethod::NumberBitAnd(Object *a,Object *b){
    auto f=a->type_obj->nb_bit_and;
    auto g=b->type_obj->nb_rbit_and;
    return BinaryOP(a,b,f,g);
}
Object *NumberMethod::NumberBitOr(Object *a,Object *b){
    auto f=a->type_obj->nb_bit_or;
    auto g=b->type_obj->nb_rbit_or;
    return BinaryOP(a,b,f,g);
}
Object *NumberMethod::NumberBitXor(Object *a,Object *b){
    auto f=a->type_obj->nb_bit_xor;
    auto g=b->type_obj->nb_rbit_xor;
    return BinaryOP(a,b,f,g);
}
Object *CompareMethod::CmpEQ(Object *a,Object *b){
    auto f=a->type_obj->cmp_eq;
    auto g=b->type_obj->cmp_eq;
    auto res=BinaryOP(a,b,f,g);
    if (res!=nullptr) return res;
    return BoolObject::Singleton(a==b);
}
Object *CompareMethod::CmpNE(Object *a,Object *b){
    auto f=a->type_obj->cmp_ne;
    auto g=b->type_obj->cmp_ne;
    auto res=BinaryOP(a,b,f,g);
    if (res!=nullptr) return res;
    return BoolObject::Singleton(a!=b);
}
Object *CompareMethod::CmpLT(Object *a,Object *b){
    auto f=a->type_obj->cmp_lt;
    auto g=b->type_obj->cmp_gt;
    return BinaryOP(a,b,f,g);
}
Object *CompareMethod::CmpLE(Object *a,Object *b){
    auto f=a->type_obj->cmp_le;
    auto g=b->type_obj->cmp_ge;
    return BinaryOP(a,b,f,g);
}
Object *CompareMethod::CmpGT(Object *a,Object *b){
    auto f=a->type_obj->cmp_gt;
    auto g=b->type_obj->cmp_lt;
    return BinaryOP(a,b,f,g);
}
Object *CompareMethod::CmpGE(Object *a,Object *b){
    auto f=a->type_obj->cmp_ge;
    auto g=b->type_obj->cmp_le;
    return BinaryOP(a,b,f,g);
}
std::vector<Object*> IterMethod::IterToVector(Object *obj){
    assert(obj->type_obj->iter_get_iter!=nullptr);
    auto iter=(*obj->type_obj->iter_get_iter)(obj);
    std::vector<Object*> vec;
    for (;;){
        auto next=(*iter->type_obj->iter_get_next)(iter);
        if (next!=nullptr) vec.push_back(next); else break;
    }
    return vec;
}
TypeObject::TypeObject(TypeObject *type):Object(type),name("type"),tp_base(nullptr){}
TypeObject::TypeObject(Object*):Object(nullptr),name("object"),tp_base(nullptr){}
TypeObject::TypeObject(const std::string &_name):TypeObject(_name,{ObjectTypeObject::Singleton()}){}
TypeObject::TypeObject(const std::string &_name,std::vector<TypeObject*> bases):Object(TypeTypeObject::Singleton()),name(_name),tp_base(bases[0]),tp_bases(bases){
    std::reverse(bases.begin(),bases.end());
    for (auto base:bases){
        for (auto it:base->tp_map){
            const auto &name=it.first;
            if (name=="__init__") continue;
            this->tp_map[name]=it.second;
        }
    }
}

Object *UnaryFunc::operator()(Object *a){
    return this->call(a);
}
Object *BinaryFunc::operator()(Object *a,Object *b){
    return this->call(a,b);
}
Object *TernaryFunc::operator()(Object *a,Object *b,Object *c){
    return this->call(a,b,c);
}

UnaryFuncPtrWrapper::UnaryFuncPtrWrapper(Object *(*_func)(Object*)):func(_func){}
Object *UnaryFuncPtrWrapper::call(Object *a){
    return this->func(a);
}

BinaryFuncPtrWrapper::BinaryFuncPtrWrapper(Object *(*_func)(Object*,Object*)):func(_func){}
Object *BinaryFuncPtrWrapper::call(Object *a,Object *b){
    return this->func(a,b);
}

TernaryFuncPtrWrapper::TernaryFuncPtrWrapper(Object *(*_func)(Object*,Object*,Object*)):func(_func){}
Object *TernaryFuncPtrWrapper::call(Object *a,Object *b,Object *c){
    return this->func(a,b,c);
}

Object *GeneralGetAttr(Object *dst,Object *key){
    if (dst->type_obj==dst) return nullptr;
    const auto &name=key->ToString();
    {
        auto it=dst->obj_map.find(name);
        if (it!=dst->obj_map.end()) return it->second;
    }
    auto type=dynamic_cast<TypeObject*>(dst);
    if (type!=nullptr){
        auto &map=type->tp_map;
        auto it=map.find(name);
        if (it!=map.end()) return it->second;
    }
    auto attr=dst->type_obj->GetAttr(key);
    if (attr!=nullptr){
        return new WrapperObject(dst,attr);
    }
    return nullptr;
}
Object *GeneralSetAttr(Object *dst,Object *key,Object*o){
    auto type=dynamic_cast<TypeObject*>(dst);
    const auto &name=key->ToString();
    if (type!=nullptr){
        type->tp_map[name]=o;
    } else{
        dst->obj_map[name]=o;
    }
    return o;
}
BinaryFunc *GetGeneralGetAttr(){
    return new BinaryFuncPtrWrapper(&GeneralGetAttr);
}
TernaryFunc *GetGeneralSetAttr(){
    return new TernaryFuncPtrWrapper(&GeneralSetAttr);
}
void TypeObject::BuiltinInit(){
    tp_map["__base__"]=this->tp_base;
    {
        std::vector<Object*> vec;
        for (auto base:this->tp_bases){
            auto obj=dynamic_cast<Object*>(base);
            assert(obj!=nullptr);
            vec.push_back(obj);
        }
        tp_map["__bases__"]=new TupleObject(vec);
    }
    if (this->nb_add!=nullptr){
        this->tp_map["__add__"]=new BinaryDescObject(this->nb_add);
    }
    if (this->tp_call!=nullptr){
        this->tp_map["__call__"]=new TernaryDescObject(this->tp_call);
    }
    if (this->tp_init!=nullptr){
        this->tp_map["__init__"]=new TernaryDescObject(this->tp_init);
    }
}
