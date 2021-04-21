#include "code_object.h"
CodeTypeObject *CodeTypeObject::Singleton(){
    static auto singleton=new CodeTypeObject();
    return singleton;
}
CodeTypeObject::CodeTypeObject():TypeObject("code"){
}
CodeObject::CodeObject(const std::vector<std::string> &_names,const std::vector<Object*> &_consts,const std::vector<std::string> &_varnames,const std::vector<int> &_varlocals,const std::vector<std::string> &_freenames,const std::vector<std::string> &_cellnames):Object(CodeTypeObject::Singleton()),names(_names),consts(_consts),varnames(_varnames),varlocals(_varlocals),freenames(_freenames),cellnames(_cellnames){
    for (const auto &name:this->cellnames){
        bool found=false;
        int i=0;
        for (const auto &varname:this->varnames){
            if (varname==name){
                //printf("%s is cell & argument\n",name.c_str());
                found=true;
                this->cellargs.push_back(i);
                break;
            }
            i++;
        }
        if (!found) this->cellargs.push_back(-1);
    }
}
void CodeObject::AddCode(int opcode,int oparg){
    this->codes.push_back(new CodeObjectUnit(opcode,oparg));
}
