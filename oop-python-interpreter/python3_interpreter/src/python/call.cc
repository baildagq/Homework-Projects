#include "call.h"
#include "eval.h"
Object *Call(Object *obj,std::vector<Object*> args,std::map<std::string,Object*> kwargs){
    for (;;){
        auto wrapper_object=dynamic_cast<WrapperObject*>(obj);
        if (wrapper_object!=nullptr){
            args.insert(args.begin(),wrapper_object->obj);
            obj=wrapper_object->attr;
        } else break;
    }

    auto builtin_func_object=dynamic_cast<BuiltinFuncObject*>(obj);
    if (builtin_func_object!=nullptr){
        return (*(builtin_func_object->callfunc))(args,{});
    }
    auto func_object=dynamic_cast<FuncObject*>(obj);
    if (func_object!=nullptr){
        if (!func_object->code->has_yield){
            auto local=new std::map<std::string,Object*>();
            auto eval=new Evaluator(func_object->code,func_object->builtins,func_object->globals,local,func_object->closure);
            for (int i=0;i<(int)args.size();i++)
                eval->SetFastLocal(func_object->code->varlocals[i],args[i]);
            return eval->Eval();
        } else{
            auto gen_object=new GeneratorObject(func_object->name,func_object->code,func_object->builtins,func_object->globals,func_object->closure);
            auto eval=new Evaluator(gen_object->code,gen_object->builtins,gen_object->globals,new std::map<std::string,Object*>(),gen_object->closure);
            for (int i=0;i<(int)args.size();i++)
                eval->SetFastLocal(func_object->code->varlocals[i],args[i]);
            gen_object->eval=eval;
            gen_object->next=[](Object *obj)->Object*{
                auto gen=dynamic_cast<GeneratorObject*>(obj);
                assert(gen!=nullptr);
                auto eval=static_cast<Evaluator*>(gen->eval);
                return eval->Eval();
            };
            return gen_object;
        }
    };
    {
        auto call=obj->type_obj->tp_call;
        if (call!=nullptr){
            return (*call)(obj,new TupleObject(args),new DictObject());
        }
    }
    {
        auto call=obj->GetAttr(new StringObject("__call__"));
        args.insert(args.begin(),obj);
        if (call!=nullptr) return Call(call,args,kwargs);
    }
    assert(false);
}
