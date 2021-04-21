#pragma once
#include "common.h"
class TypeObject;
class Object{
public:
    TypeObject *type_obj;
    Object(TypeObject *_type_obj);
    virtual std::string ToString()=0;
    virtual std::string ToRepr(){return this->ToString();}
    virtual int ToInt(){assert(false);}
    virtual bool ToBool(){assert(false);}
    virtual int GetSize(){assert(false);}
    virtual Object *GetAttr(Object*);
    virtual Object *SetAttr(Object*,Object*);
    std::map<std::string,Object*> obj_map;
    virtual ~Object(){}
};

class UnaryFunc{
    virtual Object *call(Object*)=0;
public:
    Object *operator()(Object*);
    virtual ~UnaryFunc(){}
};
class BinaryFunc{
    virtual Object *call(Object*,Object*)=0;
public:
    Object *operator()(Object*,Object*);
    virtual ~BinaryFunc(){}
};
class TernaryFunc{
    virtual Object *call(Object*,Object*,Object*)=0;
public:
    Object *operator()(Object*,Object*,Object*);
    virtual ~TernaryFunc(){}
};

class UnaryFuncPtrWrapper:public UnaryFunc{
    Object *(*func)(Object*);
    Object *call(Object*) override;
public:
    UnaryFuncPtrWrapper(Object *(*func)(Object*));
    ~UnaryFuncPtrWrapper(){}
};
class BinaryFuncPtrWrapper:public BinaryFunc{
    Object *(*func)(Object*,Object*);
    Object *call(Object*,Object*) override;
public:
    BinaryFuncPtrWrapper(Object *(*func)(Object*,Object*));
    BinaryFuncPtrWrapper(BinaryFunc *src):func(static_cast<BinaryFuncPtrWrapper*>(src)->func){}
    ~BinaryFuncPtrWrapper(){}
};
class TernaryFuncPtrWrapper:public TernaryFunc{
    Object *(*func)(Object*,Object*,Object*);
    Object *call(Object*,Object*,Object*) override;
public:
    TernaryFuncPtrWrapper(Object *(*func)(Object*,Object*,Object*));
    ~TernaryFuncPtrWrapper(){}
};

class NumberMethod{
public:
    UnaryFunc *nb_negative=nullptr;
    UnaryFunc *nb_positive=nullptr;

    BinaryFunc *nb_add=nullptr;
    BinaryFunc *nb_radd=nullptr;
    BinaryFunc *nb_inplace_add=nullptr;

    BinaryFunc *nb_subtract=nullptr;
    BinaryFunc *nb_rsubtract=nullptr;
    BinaryFunc *nb_inplace_subtract=nullptr;

    BinaryFunc *nb_multiply=nullptr;
    BinaryFunc *nb_rmultiply=nullptr;
    BinaryFunc *nb_inplace_multiply=nullptr;

    BinaryFunc *nb_floor_div=nullptr;
    BinaryFunc *nb_rfloor_div=nullptr;
    BinaryFunc *nb_inplace_floor_div=nullptr;

    BinaryFunc *nb_true_div=nullptr;
    BinaryFunc *nb_rtrue_div=nullptr;
    BinaryFunc *nb_inplace_true_div=nullptr;

    BinaryFunc *nb_module=nullptr;
    BinaryFunc *nb_rmodule=nullptr;
    BinaryFunc *nb_inplace_module=nullptr;

    BinaryFunc *nb_lshift=nullptr;
    BinaryFunc *nb_rlshift=nullptr;
    BinaryFunc *nb_inplace_lshift=nullptr;

    BinaryFunc *nb_rshift=nullptr;
    BinaryFunc *nb_rrshift=nullptr;
    BinaryFunc *nb_inplace_rshift=nullptr;

    BinaryFunc *nb_bit_and=nullptr;
    BinaryFunc *nb_rbit_and=nullptr;
    BinaryFunc *nb_inplace_bit_and=nullptr;

    BinaryFunc *nb_bit_or=nullptr;
    BinaryFunc *nb_rbit_or=nullptr;
    BinaryFunc *nb_inplace_bit_or=nullptr;

    BinaryFunc *nb_bit_xor=nullptr;
    BinaryFunc *nb_rbit_xor=nullptr;
    BinaryFunc *nb_inplace_bit_xor=nullptr;

    static Object *NumberAdd(Object*,Object*);
    static Object *NumberSubtract(Object*,Object*);
    static Object *NumberMultiply(Object*,Object*);
    static Object *NumberFloorDiv(Object*,Object*);
    static Object *NumberTrueDiv(Object*,Object*);
    static Object *NumberModule(Object*,Object*);
    static Object *NumberLShift(Object*,Object*);
    static Object *NumberRShift(Object*,Object*);
    static Object *NumberBitAnd(Object*,Object*);
    static Object *NumberBitOr(Object*,Object*);
    static Object *NumberBitXor(Object*,Object*);
    virtual ~NumberMethod();
};
class CompareMethod{
public:
    BinaryFunc *cmp_eq=nullptr;
    BinaryFunc *cmp_ne=nullptr;
    BinaryFunc *cmp_lt=nullptr;
    BinaryFunc *cmp_le=nullptr;
    BinaryFunc *cmp_gt=nullptr;
    BinaryFunc *cmp_ge=nullptr;
    static Object *CmpEQ(Object*,Object*);
    static Object *CmpNE(Object*,Object*);
    static Object *CmpLT(Object*,Object*);
    static Object *CmpLE(Object*,Object*);
    static Object *CmpGT(Object*,Object*);
    static Object *CmpGE(Object*,Object*);
    virtual ~CompareMethod();
};

class IterMethod{
public:
    UnaryFunc *iter_get_iter=nullptr;
    UnaryFunc *iter_get_next=nullptr;
    static std::vector<Object*> IterToVector(Object*);
    virtual ~IterMethod();
};


Object *GeneralGetAttr(Object*,Object*);
Object *GeneralSetAttr(Object*,Object*,Object*);
BinaryFunc *GetGeneralGetAttr();
TernaryFunc *GetGeneralSetAttr();

class TypeObject:public Object,public NumberMethod,public CompareMethod,public IterMethod{
public:
    std::string name;

    TypeObject(const std::string &_name);
    TypeObject(const std::string &_name,std::vector<TypeObject*> bases);

    TypeObject(TypeObject *type);
    TypeObject(Object *);

    std::string ToString(){return "<class '"+this->name+"'>";}
    TypeObject *tp_base=nullptr;
    std::vector<TypeObject*> tp_bases;

    TernaryFunc *tp_new=nullptr;
    TernaryFunc *tp_init=nullptr;


    TernaryFunc *tp_call=nullptr;
    UnaryFunc *tp_hash=nullptr;

    std::map<std::string,Object*> tp_map;

    BinaryFunc *tp_getattr=nullptr;
    TernaryFunc *tp_setattr=nullptr;
 
    BinaryFunc *tp_getitem=nullptr;
    TernaryFunc *tp_setitem=nullptr;

    void BuiltinInit();
    ~TypeObject();
};
