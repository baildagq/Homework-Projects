#include "dict_object.h"
#include "all_object.h"
DictTypeObject *DictTypeObject::Singleton(){
    static auto singleton=new DictTypeObject();
    return singleton;
}
DictTypeObject::DictTypeObject():TypeObject("dict"){
    this->tp_getattr=GetGeneralGetAttr();
    this->tp_setitem=new TernaryFuncPtrWrapper([](Object *obj,Object *key,Object *value)->Object*{
        auto dict=dynamic_cast<DictObject*>(obj);
        if (dict!=nullptr){
            dict->Set(key,value);
            return NoneObject::Singleton();
        }
        return nullptr;
    });
    this->tp_getitem=new BinaryFuncPtrWrapper([](Object *obj,Object *key)->Object*{
        auto dict=dynamic_cast<DictObject*>(obj);
        if (dict!=nullptr){
            return dict->Get(key);
        }
        return nullptr;
    });
    this->tp_map["keys"]=new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert((int)args.size()==1);
        assert(kwargs.empty());
        auto dict=dynamic_cast<DictObject*>(args[0]);
        std::vector<Object*> vec;
        for (auto it:dict->map)
            vec.push_back(it.first);
        return new TupleObject(vec);
    });
    this->tp_map["items"]=new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert((int)args.size()==1);
        assert(kwargs.empty());
        auto dict=dynamic_cast<DictObject*>(args[0]);
        std::vector<Object*> vec;
        for (auto it:dict->map){
            auto t=new TupleObject(2);
            t->Set(0,it.first);
            t->Set(1,it.second);
            vec.push_back(t);
        }
        return new TupleObject(vec);
    });
}
DictObject::DictObject():Object(DictTypeObject::Singleton()){}
void DictObject::Set(Object *key,Object *value){
    auto hashfunc=key->type_obj->tp_hash;
    assert(hashfunc!=nullptr);
    auto hash=dynamic_cast<IntObject*>((*hashfunc)(key));
    assert(hash!=nullptr);
    int h=hash->ToInt()%DICTMAXSIZE;
    for (auto &i:this->vecs[h]){
        auto res=CompareMethod::CmpEQ(i.first,key);
        if (res==nullptr) continue;
        if (res->ToBool()){
            i.second=value;
            return;
        }
    }
    this->vecs[h].push_back(std::make_pair(key,value));
    this->map[key]=value;
}
Object *DictObject::Get(Object *key){
    auto hashfunc=key->type_obj->tp_hash;
    assert(hashfunc!=nullptr);
    auto hash=dynamic_cast<IntObject*>((*hashfunc)(key));
    assert(hash!=nullptr);
    int h=hash->ToInt()%DICTMAXSIZE;
    for (auto &i:this->vecs[h]){
        auto res=CompareMethod::CmpEQ(i.first,key);
        if (res==nullptr) continue;
        if (res->ToBool()){
            return i.second;
        }
    }
    assert(false);
}
std::string DictObject::ToString(){
    std::string ans;
    ans+="{";
    bool first=true;
    for (int i=0;i<DICTMAXSIZE;i++){
        auto &vec=this->vecs[i];
        for (auto it:vec){
            if (first) first=false; else ans+=", ";
            ans+=it.first->ToRepr()+": "+it.second->ToRepr();
        }
    }
    ans+="}";
    return ans;
}
