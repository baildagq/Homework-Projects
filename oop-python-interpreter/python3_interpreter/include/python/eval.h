#pragma once
#include "common.h"
#include "all_object.h"
class Evaluator{
    int eval_index=0;
    bool first=true;
    CodeObject *codeobject;
    std::stack<Object*> stack;
    std::map<std::string,Object*> *builtins;
    std::map<std::string,Object*> *globals;
    std::map<std::string,Object*> *locals;
    std::vector<Object*> fastlocals;
    std::vector<RefObject*> cellvars;
    std::vector<RefObject*> freevars;
    TupleObject *closure;
    void EnsureFastLocal(int i);
public:
    void SetFastLocal(int i,Object*);
    Object *GetFastLocal(int i);
    Object *GetLocal(const std::string &name);
    Evaluator(CodeObject *_codeobject,std::map<std::string,Object*> *_builtins,std::map<std::string,Object*> *_globals,std::map<std::string,Object*> *_locals,TupleObject *_closure);
    Object *Eval();
};
