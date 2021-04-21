#pragma once
#include "object.h"
class DictTypeObject:public TypeObject{
    DictTypeObject();
public:
    static DictTypeObject *Singleton();
};
class DictObject:public Object{
    friend class DictTypeObject;
    static const int DICTMAXSIZE=10007;
    std::map<Object*,Object*> map;
    std::vector<std::pair<Object*,Object*>> vecs[DICTMAXSIZE];
public:
    DictObject();
    void Set(Object *key,Object *value);
    Object *Get(Object *key);
    std::string ToString() override;
    int GetSize() override{return (int)this->map.size();};
    bool IsEmpty(){return this->GetSize()==0;}
};
