#include "func_object.h"
#include "all_object.h"
FuncTypeObject *FuncTypeObject::Singleton(){
    static auto singleton=new FuncTypeObject();
    return singleton;
}
FuncTypeObject::FuncTypeObject():TypeObject("function"){
}
FuncObject::FuncObject(const std::string &_name,CodeObject *_code,std::map<std::string,Object*> *_builtins,std::map<std::string,Object*> *_globals,TupleObject *_closure):Object(FuncTypeObject::Singleton()),name(_name),code(_code),builtins(_builtins),globals(_globals),closure(_closure){
}
