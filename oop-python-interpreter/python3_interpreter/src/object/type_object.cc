#include "type_object.h"
#include "all_object.h"
TypeTypeObject *TypeTypeObject::Singleton(bool init){
    static auto singleton=new TypeTypeObject();
    static bool first=true;
    if (init&&first){
        singleton->tp_base=ObjectTypeObject::Singleton(false);
        first=false;
    }
    return singleton;
}
TypeTypeObject::TypeTypeObject():TypeObject(this){
    this->tp_new=new TernaryFuncPtrWrapper([](Object *type0,Object *args,Object *kwargs)->Object*{
        auto type=dynamic_cast<TypeObject*>(type0);
        auto tuple=dynamic_cast<TupleObject*>(args);
        auto dict=dynamic_cast<DictObject*>(kwargs);
        assert(type!=nullptr);
        assert(tuple!=nullptr);
        assert(dict!=nullptr);
        assert(dict->IsEmpty());
        assert(IsSubType(type,TypeTypeObject::Singleton()));
        if ((int)tuple->GetSize()==1){
            return tuple->Get(0)->type_obj;
        }
        return nullptr;
    });
    this->tp_call=new TernaryFuncPtrWrapper([](Object *type0,Object *args,Object *kwargs)->Object*{
        auto type=dynamic_cast<TypeObject*>(type0);
        auto tuple=dynamic_cast<TupleObject*>(args);
        auto dict=dynamic_cast<DictObject*>(kwargs);
        assert(type!=nullptr);
        assert(tuple!=nullptr);
        assert(dict!=nullptr);
        auto new_func=type->tp_new;
        assert(new_func!=nullptr);
        auto obj=(*new_func)(type,args,kwargs);
        if (IsInstance(obj,type)){
            auto init_func=type->tp_init;
            if (init_func!=nullptr) (*init_func)(obj,args,kwargs);
        }
        return obj;
    });
    this->tp_getattr=GetGeneralGetAttr();
    this->tp_setattr=GetGeneralSetAttr();
}
bool IsSubType(TypeObject *a,TypeObject *b){
    std::set<TypeObject*> vis;
    std::queue<TypeObject*> queue;
    queue.push(a);
    vis.insert(a);
    while (!queue.empty()){
        auto type=queue.front();queue.pop();
        for (auto base:type->tp_bases)
            if (vis.find(base)==vis.end()){
                queue.push(base);
                vis.insert(base);
            }
    }
    for (auto i:vis) if (i==b) return true;
    return false;
}
bool IsInstance(Object *a,TypeObject *b){return IsSubType(a->type_obj,b);}
