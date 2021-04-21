#pragma once
#include "common.h"
class Node{
public:
    int type;
    std::string str;
    std::vector<Node*> childs;
    int lineno;
    int col_offset;
    Node(int _type,const std::string &_str,const std::vector<Node*> &_childs,int _lineno,int _col_offset):type(_type),str(_str),childs(_childs),lineno(_lineno),col_offset(_col_offset){}
};
