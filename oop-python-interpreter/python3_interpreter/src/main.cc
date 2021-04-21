#include "graminit.h"
#include "tokenizer.h"
#include "parser.h"
#include "ast.h"
#include "compile.h"
#include "eval.h"
#include "all_object.h"
FILE *openfile(const char *filename,const char *mode){
    FILE *fp=fopen(filename,mode);
    if (fp==nullptr){
        perror(filename);
        exit(1);
    }
    return fp;
}
std::string trans(const std::string &str){
    std::string ans="";
    for (int i=0;i<(int)str.size();i++){
        if (str[i]=='\n') ans+="\\n";
        else ans+=str[i];
    }
    return ans;
}
void print(Node *p){
    static int c=0;
    assert(p!=nullptr);
    if ((int)p->childs.size()==1){
        if (0){
            for (Node *q:p->childs) print(q);
            return;
        }
    }
    for (int i=0;i<c;i++) printf("    ");
    if ((int)p->childs.size()==1) putchar('|');
    printf("type:%s str:\"%s\" r:%d c:%d nr_child:%d\n",(p->type>NT_OFFSET?NonTerminalMap(p->type):std::to_string(p->type)).c_str(),trans(p->str).c_str(),p->lineno,p->col_offset,(int)p->childs.size());
    if (p->childs.empty()) return;
    bool flag=false;
    if ((int)p->childs.size()>1) c++,flag=true;
    for (Node *q:p->childs){
        print(q);
    }
    if (flag) c--;
}
std::map<std::string,Object*> *GetBuiltin(){
    auto map=new std::map<std::string,Object*>();
    (*map)["__name__"]=new StringObject("__main__");

    auto printfunc=new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        std::string sep=" ";
        std::string end="\n";
        for (auto it:kwargs){
            if (it.first=="sep"){
                auto str=dynamic_cast<StringObject*>(it.second);
                if (str!=nullptr)
                    sep=str->ToString();
                else if (it.second!=NoneObject::Singleton())
                    throw CustomException("end must be None or a string");
            } else if (it.first=="end"){
                auto str=dynamic_cast<StringObject*>(it.second);
                if (str!=nullptr)
                    end=str->ToString();
                else if (it.second!=NoneObject::Singleton())
                    throw CustomException("end must be None or a string");
            } else{
                assert(false);
            }
        }
        bool first=true;
        for (auto o:args){
            if (!first) printf("%s",sep.c_str()); else first=false;
            printf("%s",o->ToString().c_str());
        }
        printf("%s",end.c_str());
        return NoneObject::Singleton();
    });
    (*map)["print"]=printfunc;

    auto rangefunc=new BuiltinFuncObject([](std::vector<Object*> vec,std::map<std::string,Object*> kwargs)->Object*{
        assert(kwargs.empty());
        switch((int)vec.size()){
            case 1:
                return new RangeObject(vec[0]->ToInt());
            case 2:
                return new RangeObject(vec[0]->ToInt(),vec[1]->ToInt());
            case 3:
                return new RangeObject(vec[0]->ToInt(),vec[1]->ToInt(),vec[2]->ToInt());
            default:
                assert(false);
        }
    });
    (*map)["range"]=rangefunc;

    (*map)["int"]=IntTypeObject::Singleton();
    (*map)["str"]=StringTypeObject::Singleton();

    (*map)["__build_class__"]=new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert(kwargs.empty());
        switch((int)args.size()){
            case 3:{
                auto func=dynamic_cast<FuncObject*>(args[0]);
                assert(func!=nullptr);
                auto bases=dynamic_cast<TupleObject*>(args[2]);
                assert(bases!=nullptr);
                std::vector<TypeObject*> vec;
                for (int i=0;i<bases->GetSize();i++){
                    auto type=dynamic_cast<TypeObject*>(bases->Get(i));
                    assert(type!=nullptr);
                    vec.push_back(type);
                }
                auto type=new CustomTypeObject(args[1]->ToString(),vec);
                auto evaluator=new Evaluator(func->code,func->builtins,func->globals,new std::map<std::string,Object*>(),func->closure);
                evaluator->Eval();
                for (auto name:func->code->names){
                    //printf("name:%s\n",name.c_str());
                    type->SetAttr(new StringObject(name),evaluator->GetLocal(name));
                }
                return type;
            }
            default:
                assert(false);
        }
    });

    (*map)["len"]=new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert(kwargs.empty());
        assert((int)args.size()==1);
        auto obj=args[0];
        return new IntObject(obj->GetSize());
    });
    (*map)["sum"]=new BuiltinFuncObject([](std::vector<Object*> args,std::map<std::string,Object*> kwargs)->Object*{
        assert(kwargs.empty());
        assert((int)args.size()>=1);
        assert((int)args.size()<=2);
        auto sum=((int)args.size()==2)?args[1]:new IntObject(0);
        for (auto obj:IterMethod::IterToVector(args[0]))
            sum=NumberMethod::NumberAdd(sum,obj);
        return sum;
    });

    (*map)["object"]=ObjectTypeObject::Singleton();
    (*map)["int"]=IntTypeObject::Singleton();
    (*map)["str"]=StringTypeObject::Singleton();
    (*map)["bool"]=BoolTypeObject::Singleton();
    (*map)["tuple"]=TupleTypeObject::Singleton();
    (*map)["list"]=ListTypeObject::Singleton();
    (*map)["type"]=TypeTypeObject::Singleton();

    return map;
}
int main(int argc,char *argv[]){
    if (argc!=2){
        printf("usage:%s test.py\n",argv[0]);
        return 0;
    }
    try{
        auto fp=openfile(argv[1],"r");
        auto g=GetGrammar();
        auto parser=new Parser(g,file_input);
        auto n=parser->ParseFile(fp);
        //print(n);
        auto ast=new AST();
        auto mod=ast->FromNode(n);
        auto compiler=new Compiler();
        auto codeobject=compiler->Compile(mod);
        auto globals=new std::map<std::string,Object*>();
        auto eval=new Evaluator(codeobject,GetBuiltin(),globals,globals,new TupleObject(0));
        auto res=eval->Eval();
        assert(res==NoneObject::Singleton());
        //puts("");
        //printf("return:%s\n",res->ToString().c_str());
    } catch (std::exception &issue){
        printf("%s\n",issue.what());
    }
    return 0;
}
