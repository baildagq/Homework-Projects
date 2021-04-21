#include <algorithm>
#include "list_object.h"
#include "all_object.h"
ListTypeObject *ListTypeObject::Singleton(){
	static auto singleton=new ListTypeObject();
	return singleton;
}
ListTypeObject::ListTypeObject():TypeObject("list"){
    this->tp_getattr=GetGeneralGetAttr();
    this->tp_setattr=GetGeneralSetAttr();
    this->tp_new=new TernaryFuncPtrWrapper([](Object *type,Object *args,Object *kwargs)->Object*{
        return new ListObject(0);
    });
    this->tp_init=new TernaryFuncPtrWrapper([](Object *obj,Object *args,Object *kwargs)->Object*{
        auto list=dynamic_cast<ListObject*>(obj);
        auto tuple=dynamic_cast<TupleObject*>(args);
        auto dict=dynamic_cast<DictObject*>(kwargs);
        assert(list!=nullptr);
        assert(tuple!=nullptr);
        assert(dict!=nullptr);
        assert(tuple->GetSize()==1);
        assert(dict->IsEmpty());
        auto iter_obj=tuple->Get(0);
        assert(iter_obj->type_obj->iter_get_iter!=nullptr);
        auto iter=(*iter_obj->type_obj->iter_get_iter)(iter_obj);
        assert(iter->type_obj->iter_get_next!=nullptr);
        assert(iter!=nullptr);
        for (;;){
            auto obj=(*iter->type_obj->iter_get_next)(iter);
            if (obj!=nullptr) list->PushBack(obj); else break;
        }
        return NoneObject::Singleton();
    });
    this->iter_get_iter=new UnaryFuncPtrWrapper([](Object *oa)->Object*{
        auto a=dynamic_cast<ListObject*>(oa);
        assert(a!=nullptr);
        return new ListIterObject(a);
    });
    this->tp_setitem=new TernaryFuncPtrWrapper([](Object *obj,Object *sub,Object *v)->Object*{
        {
            auto list=dynamic_cast<ListObject*>(obj);
            auto index=dynamic_cast<IntObject*>(sub);
            if (list!=nullptr&&index!=nullptr){
                list->Set(index->ToInt(),v);
                return NoneObject::Singleton();
            }
        }
        assert(false);
    });
    this->nb_add=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<ListObject*>(oa);
        auto b=dynamic_cast<ListObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            auto res=new ListObject(0);
            for (auto obj:a->ToVector())
                res->PushBack(obj);
            for (auto obj:b->ToVector())
                res->PushBack(obj);
            return res;
        }
        return nullptr;
    });
    this->nb_inplace_add=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<ListObject*>(oa);
        auto b=dynamic_cast<ListObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            for (auto obj:b->ToVector())
                a->PushBack(obj);
            return a;
        }
        return nullptr;
    });
    this->nb_multiply=new BinaryFuncPtrWrapper([](Object *a,Object *b)->Object*{
        auto list=dynamic_cast<ListObject*>(a);
        if (list!=nullptr){
            auto res=new ListObject(0);
            for (int i=0;i<b->ToInt();i++)
                for (auto obj:list->ToVector())
                    res->PushBack(obj);
            return res;
        }
        return nullptr;
    });
    this->SetAttr(new StringObject("__setitem__"),new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert((int)args.size()==3);
        auto list=dynamic_cast<ListObject*>(args[0]);
        auto i=dynamic_cast<IntObject*>(args[1]);
        list->Set(i->ToInt(),args[2]);
        return NoneObject::Singleton();
    }));
    this->SetAttr(new StringObject("__getitem__"),new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert((int)args.size()==2);
        auto list=dynamic_cast<ListObject*>(args[0]);
        auto i=dynamic_cast<IntObject*>(args[1]);
        return list->Get(i->ToInt());
    }));
    this->SetAttr(new StringObject("append"),new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert((int)args.size()==2);
        auto list=dynamic_cast<ListObject*>(args[0]);
        assert(list!=nullptr);
        list->PushBack(args[1]);
        return NoneObject::Singleton();
    }));
    this->SetAttr(new StringObject("pop"),new BuiltinFuncObject([](std::vector<Object*> args, std::map<std::string,Object*> kwargs)->Object*{
        if((int)args.size()==1){
            auto list=dynamic_cast<ListObject*>(args[0]);
            return list->Pop();
        }else if((int)args.size()==2){
            auto list=dynamic_cast<ListObject*>(args[0]);
            return list->Pop(args[1]);
        }
        assert(false);
    }));
    this->SetAttr(new StringObject("count"),new BuiltinFuncObject([](std::vector<Object*> args, std::map<std::string,Object*> kwargs)->Object*{
        assert(args.size()==2);
        auto list=dynamic_cast<ListObject*>(args[0]);
        assert(list!=nullptr);
        return list->Count(args[1]);
    }));
    this->SetAttr(new StringObject("reverse"),new BuiltinFuncObject([](std::vector<Object*> args, std::map<std::string,Object*> kwargs)->Object*{
        assert((int)args.size()==1);
        auto list=dynamic_cast<ListObject*>(args[0]);
        list->Reverse();
        return NoneObject::Singleton();
    }));
    this->SetAttr(new StringObject("clear"),new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert((int)args.size()==1);
        auto list=dynamic_cast<ListObject*>(args[0]);
        assert(list!=nullptr);
        list->Clear();
        return NoneObject::Singleton();
    }));
    this->SetAttr(new StringObject("copy"),new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert((int)args.size()==1);
        auto list=dynamic_cast<ListObject*>(args[0]);
        assert(list!=nullptr);
        return new ListObject(list->ToVector());
    }));
    this->SetAttr(new StringObject("insert"),new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert((int)args.size()==3);
        auto list=dynamic_cast<ListObject*>(args[0]);
        list->Insert(args[1],args[2]);
        return NoneObject::Singleton();
    }));
    this->SetAttr(new StringObject("remove"),new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert((int)args.size()==2);
        auto list=dynamic_cast<ListObject*>(args[0]);
        list->Remove(args[1]);
        return NoneObject::Singleton();
    }));
}
ListObject::ListObject(int n):Object(ListTypeObject::Singleton()),vec(n,nullptr){}
ListObject::ListObject(const std::vector<Object*>& v):Object(ListTypeObject::Singleton()){
    vec.assign(v.begin(),v.end());
}
std::vector<Object*>& ListObject::ToVector(){
    return vec;
}
void ListObject::Set(int i,Object *obj){
    this->vec[i]=obj;
}
Object *ListObject::Get(int i){
    return this->vec[i];
}
int ListObject::Size(){
    return (int)this->vec.size();
}
void ListObject::PushBack(Object *obj){
    this->vec.push_back(obj);
}
Object *ListObject::Pop(Object *obj){
	if(obj == nullptr){
        auto obj=vec.end()[-1];
		vec.pop_back();
        return obj;
	}
	else{
		IntObject *i = dynamic_cast<IntObject*>(obj);
		assert(i != nullptr);
        auto obj=vec[i->ToInt()];
		vec.erase(vec.begin() + i->ToInt(), vec.begin() + i->ToInt() + 1);
        return obj;
	}
}
Object* ListObject::Count(Object* b){
    int ans=0;
    for(auto item:this->vec){
        auto eq=b->type_obj->cmp_eq;
        if (eq==nullptr) break;
        auto res=(*eq)(b,item);
        if (res==nullptr) break;
        if (res->ToBool()) ans ++;
    }
    return new IntObject(ans);
}
void ListObject::Reverse(){
    reverse(vec.begin(),vec.end());
}
void ListObject::Clear(){
    vec.clear();
}
void ListObject::Insert(Object* i,Object* obj){
    auto oi=dynamic_cast<IntObject*>(i);
    assert(oi!=nullptr);
    vec.insert(vec.begin()+oi->ToInt(),obj);
}
void ListObject::Remove(Object* obj){
    int count=0;
    for(auto item:this->vec){
        auto res=CompareMethod::CmpEQ(obj,item);
        if (res==nullptr) break;
        if (res->ToBool()){
            vec.erase(vec.begin()+count);
            return;
        }
        count ++;
    }
    assert(false);
}
Object *ListTypeObject::Set(Object *oa, Object  *oi, Object *ob){
	ListObject *a = dynamic_cast<ListObject*>(oa);
	IntObject *i = dynamic_cast<IntObject*>(oi);
	assert(a != nullptr);
	assert(i != nullptr);
	a->vec[i->ToInt()] = ob;
	return NULL;
}
Object *ListTypeObject::Get(Object *oa, Object *oi){
	ListObject *a = dynamic_cast<ListObject*>(oa);
	IntObject *i = dynamic_cast<IntObject*>(oi);
	assert(a != nullptr);
	assert(i != nullptr);
	return a->vec[i->ToInt()];
}
Object *ListTypeObject::Size(Object *oa){
	ListObject *a = dynamic_cast<ListObject*>(oa);
	assert(a != nullptr);
	return new IntObject((int)a->vec.size());
}
Object *ListTypeObject::Append(Object *oa, Object *o){
	ListObject *a = dynamic_cast<ListObject*>(oa);
	assert(a != nullptr);
	a->vec.push_back(o);
	return NULL;
}
Object *ListTypeObject::Pop(Object *oa, Object *oi ){
	ListObject *a = dynamic_cast<ListObject*>(oa);
	assert(a != nullptr);
	if(oi == nullptr){
		a->vec.pop_back();
	}
	else{
		IntObject *i = dynamic_cast<IntObject*>(oi);
		assert(i != nullptr);
		a->vec.erase(a->vec.begin() + i->ToInt(), a->vec.begin() + i->ToInt() + 1);
	}
	return nullptr;
}
Object *ListTypeObject::Count(Object *oa, Object *o){
	ListObject *a = dynamic_cast<ListObject*>(oa);
	assert(a != nullptr);
	int count = 0;
	for(int i = 0;i < (int)a->vec.size();i ++)
		if(a->vec[i] == o)
			count ++;
	return new IntObject(count);
}
Object *ListTypeObject::Reverse(Object *oa){
	ListObject *a = dynamic_cast<ListObject*>(oa);
	assert(a != nullptr);
	std::vector<Object* > temp;
	int len = a->vec.size();
	for(int i = 0;i < len;i ++)
		temp.push_back(a->vec[len - i - 1]);
	for(int i = 0;i < len;i ++)
		a->vec[i] = temp[i];
	return NULL;
}
std::string ListObject::ToString(){
    static std::set<Object*> vis;
    if (vis.find(this)!=vis.end()) return "[...]";
    vis.insert(this);
    std::string ans;
    ans+="[";
    bool first=true;
    for (auto obj:this->vec){
        if (first) first=false; else ans+=", ";
        ans+=obj->ToRepr();
    }
    ans+="]";
    vis.erase(this);
    return ans;
}
ListIterTypeObject *ListIterTypeObject::Singleton(){
    static auto singleton=new ListIterTypeObject();
    return singleton;
}
ListIterTypeObject::ListIterTypeObject():TypeObject("list_iterator"){
    this->iter_get_next=new UnaryFuncPtrWrapper([](Object *oa)->Object*{
        auto a=dynamic_cast<ListIterObject*>(oa);
        assert(a!=nullptr);
        int x=a->x;
        if (x>=a->list_obj->Size()) return nullptr;
        a->x++;
        return a->list_obj->Get(x);
    });
}
ListIterObject::ListIterObject(ListObject *_list_obj):Object(ListIterTypeObject::Singleton()),list_obj(_list_obj){}
