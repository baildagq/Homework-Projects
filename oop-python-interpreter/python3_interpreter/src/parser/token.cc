#include "token.h"
namespace{
std::map<std::string,int> *token_map=nullptr;
void Init(){
    token_map=new std::map<std::string,int>();
    (*token_map)["ENDMARKER"]=ENDMARKER;
    (*token_map)["NAME"]=NAME;
    (*token_map)["NUMBER"]=NUMBER;
    (*token_map)["STRING"]=STRING;
    (*token_map)["NEWLINE"]=NEWLINE;
    (*token_map)["INDENT"]=INDENT;
    (*token_map)["DEDENT"]=DEDENT;
}
}
int TokenMap(std::string str){
    if (token_map==nullptr){
        Init();
    }
    auto iter=token_map->find(str);
    if (iter==token_map->end()) return -1;
    return iter->second;
}
