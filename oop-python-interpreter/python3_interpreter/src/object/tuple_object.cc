#include "tuple_object.h"
#include "all_object.h"
namespace{
    Object *Count(std::vector<Object*> args,std::map<std::string,Object*> kwargs){
        assert((int)args.size()==2);
        auto tuple=dynamic_cast<TupleObject*>(args[0]);
        return tuple->Count(args[1]);
    }
}
TupleTypeObject *TupleTypeObject::Singleton(){
    static auto singleton=new TupleTypeObject();
    return singleton;
}
TupleTypeObject::TupleTypeObject():TypeObject("tuple"){
    this->tp_getattr=GetGeneralGetAttr();
    this->iter_get_iter=new UnaryFuncPtrWrapper([](Object *oa)->Object*{
        auto a=dynamic_cast<TupleObject*>(oa);
        assert(a!=nullptr);
        return new TupleIterObject(a);
    });
    this->tp_hash=new UnaryFuncPtrWrapper([](Object *obj)->Object*{
        auto tuple=dynamic_cast<TupleObject*>(obj);
        assert(tuple!=nullptr);
        int ans=0;
        for (auto i:tuple->ToVector()){
            auto hash=i->type_obj->tp_hash;
            assert(hash!=nullptr);
            auto h=dynamic_cast<IntObject*>((*hash)(i));
            assert(h!=nullptr);
            ans=(ans*13331ll+h->ToInt())%1000000007;
        }
        return new IntObject(ans);
    });
    this->SetAttr(new StringObject("count"),new BuiltinFuncObject(&Count));
    this->tp_getitem=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        {
            auto tuple=dynamic_cast<TupleObject*>(oa);
            auto index=dynamic_cast<IntObject*>(ob);
            if (tuple!=nullptr&&index!=nullptr){
                return tuple->Get(index->ToInt());
            }
        }
        {
            auto tuple=dynamic_cast<TupleObject*>(oa);
            auto slice=dynamic_cast<SliceObject*>(ob);
            if (tuple!=nullptr&&slice!=nullptr){
                std::vector<Object*> vec;
                int size=tuple->GetSize();
                int lower,upper,step;
                int lowbound,upbound;
                bool dir_right=true;
                if(slice->step==nullptr){
                    //printf("%s\n","step is null");
                    step=1;
                }
                else{
                    step=*slice->step;
                    if(step<0)
                        dir_right=false;
                }
                if(!dir_right){
                    lowbound=-1; upbound=lowbound+size;
                }else{
                    lowbound=0; upbound=size;
                }
                if(slice->lower==nullptr){
                    //printf("%s\n","lower is null");
                    lower= !dir_right ? upbound : lowbound;
                }else{
                    lower=*slice->lower;
                    if(lower < 0){
                        lower+=size;
                        if(lower<lowbound)
                            lower=lowbound;
                    }else{
                        if(lower>upbound){
                            lower=upbound;
                        }
                    }
                }
                if(slice->upper==nullptr){
                    //printf("%s\n","upper is null");
                    upper= !dir_right ? lowbound : upbound;
                }else{
                    upper=*slice->upper;
                    if(upper<0){
                        upper += size;
                        if(upper<lowbound)
                            upper=lowbound;
                    }
                    else{
                        if(upper>upbound)
                            upper=upbound;
                    }
                }
                //printf("%s%d\n%s%d\n%s%d\n","lower:",lower,"upper:",upper,"step:",step);
                if(dir_right){
                for(int i=lower;i<upper;i+=step){
                    //printf("%s%d\n","i=",i);
                    vec.push_back(tuple->Get(i));
                }
                }else{
                    for(int i=lower;i>upper;i+=step){
                    vec.push_back(tuple->Get(i));
                    //printf("%s%d\n","i=",i);
                    }
                }
                auto result=new TupleObject(vec);
                //printf("%s%d\n","size of new tuple: ",result->GetSize());
                return result;
            }
        assert(false);
    }
    });
}
TupleObject::TupleObject(int n):Object(TupleTypeObject::Singleton()),vec(n,nullptr){
}
TupleObject::TupleObject(const std::vector<Object*> &_vec):Object(TupleTypeObject::Singleton()),vec(_vec){
}
std::string TupleObject::ToString(){
    std::string ans;
    ans+="(";
    bool first=true;
    for (auto obj:this->vec){
        if (first) first=false; else ans+=", ";
        ans+=obj->ToRepr();
    }
    ans+=")";
    return ans;
}
void TupleObject::Set(int i,Object *o){
    this->vec[i]=o;
}
Object *TupleObject::Get(int i){
    return this->vec[i];
}
int TupleObject::GetSize(){
    return (int)this->vec.size();
}
std::vector<Object*> TupleObject::ToVector(){
    return this->vec;
}
Object *TupleObject::Count(Object *obj){
    int ans=0;
    for (auto item:this->vec){
        auto res=CompareMethod::CmpEQ(item,obj);
        if (res==nullptr) break;
        if (res->ToBool()) ans++;
    }
    return new IntObject(ans);
}

TupleIterTypeObject *TupleIterTypeObject::Singleton(){
    static auto singleton=new TupleIterTypeObject();
    return singleton;
}
TupleIterTypeObject::TupleIterTypeObject():TypeObject("tuple_iterator"){
    this->iter_get_next=new UnaryFuncPtrWrapper([](Object *oa)->Object*{
        auto a=dynamic_cast<TupleIterObject*>(oa);
        assert(a!=nullptr);
        auto x=a->x;
        if (x>=a->tuple_obj->GetSize()) return nullptr;
        a->x++;
        return a->tuple_obj->Get(x);
    });
}
TupleIterObject::TupleIterObject(TupleObject *_tuple_obj):Object(TupleIterTypeObject::Singleton()),tuple_obj(_tuple_obj){
}
