#include "generator_object.h"
#include "all_object.h"
GeneratorTypeObject *GeneratorTypeObject::Singleton(){
    static auto singleton=new GeneratorTypeObject();
    return singleton;
}
GeneratorTypeObject::GeneratorTypeObject():TypeObject("generator"){
    this->iter_get_iter=new UnaryFuncPtrWrapper([](Object *obj)->Object*{
        auto gen=dynamic_cast<GeneratorObject*>(obj);
        assert(gen!=nullptr);
        return gen;
    });
    this->iter_get_next=new UnaryFuncPtrWrapper([](Object *obj)->Object*{
        auto gen=dynamic_cast<GeneratorObject*>(obj);
        assert(gen!=nullptr);
        return (*gen->next)(gen);
    });
}
GeneratorObject::GeneratorObject(const std::string &_name,CodeObject *_code,std::map<std::string,Object*> *_builtins,std::map<std::string,Object*> *_globals,TupleObject *_closure):Object(GeneratorTypeObject::Singleton()),name(_name),code(_code),builtins(_builtins),globals(_globals),closure(_closure){
}
