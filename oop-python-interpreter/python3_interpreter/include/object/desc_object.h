#pragma once
#include "object.h"
class DescTypeObject:public TypeObject{
    DescTypeObject();
public:
    static DescTypeObject *Singleton();
};
class DescObject:public Object{
    friend DescTypeObject;
public:
    DescObject();
    virtual Object *call(const std::vector<Object*>&)=0;
    std::string ToString() override{return "desc";}
};
class BinaryDescObject:public DescObject{
    BinaryFunc *func;
public:
    BinaryDescObject(BinaryFunc *func);
    Object *call(const std::vector<Object*>&) override;
};
class TernaryDescObject:public DescObject{
    TernaryFunc *func;
public:
    TernaryDescObject(TernaryFunc *func);
    Object *call(const std::vector<Object*>&) override;
};
