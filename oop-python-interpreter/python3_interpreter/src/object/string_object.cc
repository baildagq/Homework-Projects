#include "string_object.h"
#include "all_object.h"
StringTypeObject *StringTypeObject::Singleton(){
    static auto singleton=new StringTypeObject();
    return singleton;
}
StringTypeObject::StringTypeObject():TypeObject("str"){
    this->tp_new=new TernaryFuncPtrWrapper([](Object *type,Object *args,Object *kwargs)->Object*{
        return new StringObject("");
    });
    this->tp_init=new TernaryFuncPtrWrapper([](Object *obj,Object *args,Object *kwargs)->Object*{
        auto str=dynamic_cast<StringObject*>(obj);
        auto tuple=dynamic_cast<TupleObject*>(args);
        auto dict=dynamic_cast<DictObject*>(kwargs);
        assert(str!=nullptr);
        assert(tuple!=nullptr);
        assert(dict!=nullptr);
        assert(tuple->GetSize()==1);
        str->str=tuple->Get(0)->ToString();
        return NoneObject::Singleton();
    });
    this->tp_getattr=GetGeneralGetAttr();

    this->tp_hash=new UnaryFuncPtrWrapper([](Object *obj)->Object*{
        auto str=dynamic_cast<StringObject*>(obj);
        if (str!=nullptr){
            int ans=0;
            for (auto c:str->ToString())
                ans=(ans*131ll+c)%1000000007;
            return new IntObject(ans);
        }
        return nullptr;
    });
    this->nb_add=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<StringObject*>(oa);
        auto b=dynamic_cast<StringObject*>(ob);
        assert(a!=nullptr);
        assert(b!=nullptr);
        return new StringObject(a->str+b->str);
    });
    this->nb_inplace_add=new BinaryFuncPtrWrapper(this->nb_add);
    this->nb_module=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a = dynamic_cast<StringObject*>(oa);
        auto b = dynamic_cast<TupleObject*>(ob);
        assert(a!=nullptr);
        if (b==nullptr){
            b=new TupleObject(1);
            b->Set(0,ob);
        }
        std::string str=a->ToString();
        std::string temp = "";
        int l = 0;
        for(int i = 0;i < b->GetSize();i ++){
            int r = str.find("%",l);
            temp+=str.substr(l,r-l);
            if (r+1<(int)str.size()&&str[r+1]=='d'){
                temp += std::to_string(b->Get(i)->ToInt());
                l=r+2;
            } else if (r+1<(int)str.size()&&str[r+1]=='s'){
                temp += b->Get(i)->ToString();
                l=r+2;
            } else{
                temp.push_back('%');
                l=r+1;
            }
        }
        temp+=str.substr(l,b->GetSize()-l);
        return new StringObject(temp);
    });
    this->tp_getitem=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        {
            StringObject *a=dynamic_cast<StringObject*>(oa);
            IntObject *b=dynamic_cast<IntObject*>(ob);
            if (a!=nullptr&&b!=nullptr){
                std::string s(1, a->str[b->ToInt()]);
                return new StringObject(s);
            }
        }
        {
            StringObject *a=dynamic_cast<StringObject*>(oa);
            SliceObject *b=dynamic_cast<SliceObject*>(ob);
            if (a!=nullptr&&b!=nullptr){
                std::string s="";
                int size=a->str.length();
                int lower,upper,step;
                bool dir_right = true;
                if(b->step==nullptr)
                    step=1;
                else{
                     step = *b->step;
                }
                if(step<0){
                    dir_right=false;
                    step *= -1;
                }
                if(b->lower==nullptr){
                    if(!dir_right){
                        lower = size-1;
                    }
                    else
                        lower=0;
                }
                else{
                    lower=*b->lower;
                }
                if(b->upper==nullptr){
                    if(!dir_right){
                        upper = -1-size;
                    }
                    else
                        upper=size;
                }
                else{
                    upper=*b->upper;
                }
                if(lower<0)
                    lower += size;
                if(upper<0)
                    upper += size;
                if(lower>=size){
                    if(dir_right)
                        lower=size;
                    else
                        lower=size-1;
                }
                else if(lower<0){
                    if(dir_right)
                        lower=0;
                    else
                        lower=-1;
                }
                if(upper>size){
                    if(dir_right)
                        upper=size;
                    else
                        upper=size-1;
                }
                else if(upper<0){
                    if(dir_right)
                        upper=size-1;
                    else
                        upper=-1;
                }
                //printf("%s%d%d%d\n","slice: ",lower,upper,step);
                if((dir_right && lower>upper)||(!dir_right && lower<upper))
                    ;
                else if(lower==upper){
                }
                else{
                    if(dir_right){
                        for(int i=lower;i<upper;i+=step){
                            std::string c(1,a->str[i]);
                            s += c;
                        }
                    }
                    else{
                        for(int i=lower;i>upper;i-=step){
                            std::string c(1,a->str[i]);
                            s += c;
                        }
                    }
                }
                return new StringObject(s);
            }
        }
        assert(false);
    });
    this->cmp_eq=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        StringObject *a=dynamic_cast<StringObject*>(oa);
        StringObject *b=dynamic_cast<StringObject*>(ob);
        if (a!=nullptr&&b!=nullptr){
            return BoolObject::Singleton(a->str==b->str);
        }
        return BoolObject::Singleton(false);
    });
    this->cmp_ne=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<StringObject*>(oa);
        auto b=dynamic_cast<StringObject*>(ob);
        assert(a!=nullptr);
        assert(b!=nullptr);
        return BoolObject::Singleton(a->str!=b->str);
    });
    this->cmp_lt=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<StringObject*>(oa);
        auto b=dynamic_cast<StringObject*>(ob);
        assert(a!=nullptr);
        assert(b!=nullptr);
        return BoolObject::Singleton(a->str<b->str);
    });
    this->cmp_le=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<StringObject*>(oa);
        auto b=dynamic_cast<StringObject*>(ob);
        assert(a!=nullptr);
        assert(b!=nullptr);
        return BoolObject::Singleton(a->str<=b->str);
    });
    this->cmp_gt=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<StringObject*>(oa);
        auto b=dynamic_cast<StringObject*>(ob);
        assert(a!=nullptr);
        assert(b!=nullptr);
        return BoolObject::Singleton(a->str>b->str);
    });
    this->cmp_ge=new BinaryFuncPtrWrapper([](Object *oa,Object *ob)->Object*{
        auto a=dynamic_cast<StringObject*>(oa);
        auto b=dynamic_cast<StringObject*>(ob);
        assert(a!=nullptr);
        assert(b!=nullptr);
        return BoolObject::Singleton(a->str>=b->str);
    });
    this->iter_get_iter=new UnaryFuncPtrWrapper([](Object *oa)->Object*{
        auto a=dynamic_cast<StringObject*>(oa);
        assert(a!=nullptr);
        return new StringIterObject(a);
    });
}
std::string StringObject::ToString(){
    return str;
}
std::string StringObject::ToRepr(){
    return '\''+str+'\'';
}
std::string StringObject::Get(int x){
    std::string c(1,str[x]);
    return c;
}
int StringObject::GetSize(){
    return (int)str.size();
}
StringObject::StringObject(const std::string &_str,bool from_text):Object(StringTypeObject::Singleton()),str(_str){
    if (from_text){
        int n=(int)str.size();
        if (n>=6){
            if (str[0]=='\''&&str[1]=='\''&&str[2]=='\''&&str[n-3]=='\''&&str[n-2]=='\''&&str[n-1]=='\''){
                str=str.substr(3,n-6);
                return;
            }
            if (str[0]=='"'&&str[1]=='"'&&str[2]=='"'&&str[n-3]=='"'&&str[n-2]=='"'&&str[n-1]=='"'){
                str=str.substr(3,n-6);
                return;
            }
        }
        if (str[0]=='\''&&str[n-1]=='\''){
            str=str.substr(1,n-2);
            return;
        } else{
            if (str[0]=='"'&&str[n-1]=='"'){
                str=str.substr(1,n-2);
                return;
            }
        }
    }
}
StringIterTypeObject *StringIterTypeObject::Singleton(){
    static auto singleton=new StringIterTypeObject();
    return singleton;
}
StringIterTypeObject::StringIterTypeObject():TypeObject("string_iterator"){
    this->iter_get_next=new UnaryFuncPtrWrapper([](Object *oa)->Object*{
        auto a=dynamic_cast<StringIterObject*>(oa);
        assert(a!=nullptr);
        auto x=a->x;
        if(x>=a->string_obj->GetSize()) return nullptr;
        a->x++;
        return new StringObject(a->string_obj->Get(x));
    });
}
StringIterObject::StringIterObject(StringObject *_string_obj):Object(StringIterTypeObject::Singleton()),string_obj(_string_obj){}
