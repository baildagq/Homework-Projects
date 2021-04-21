#pragma once
#include "object.h"
class CodeTypeObject:public TypeObject{
    CodeTypeObject();
public:
    static CodeTypeObject *Singleton();
};
class CodeObject:public Object{
    struct CodeObjectUnit{
        int opcode;
        int oparg;
        CodeObjectUnit(int _opcode,int _oparg):opcode(_opcode),oparg(_oparg){}
    };
public:
    bool has_yield=false;
    std::vector<CodeObjectUnit*> codes;
    std::vector<std::string> names;
    std::vector<Object*> consts;
    std::vector<std::string> varnames;
    std::vector<int> varlocals;
    std::vector<std::string> freenames;
    std::vector<std::string> cellnames;
    std::vector<int> cellargs;
    CodeObject(const std::vector<std::string> &_names,const std::vector<Object*> &_consts,const std::vector<std::string> &_varnames,const std::vector<int> &_varlocals,const std::vector<std::string> &_freenames,const std::vector<std::string> &_cellnames);
    void AddCode(int opcode,int oparg);
    std::string ToString(){return "CodeObject";}
};
