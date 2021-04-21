#pragma once
#include "common.h"
struct ASDLToken{
    int type;
    std::string str;
    int lineno;
    int col_offset;
    ASDLToken(int _type,const std::string _str,int _lineno,int _col_offset):type(_type),str(_str),lineno(_lineno),col_offset(_col_offset){}
};
class ASDLTokenizer{
    FILE *fp;
    int lineno;
    int col_offset;
    bool done=false;
    static const int BUFSIZE=1000;
    char *buf=nullptr,*cur=nullptr;
    char NextChar();
    void BackChar(char c);
public:
    ASDLTokenizer(FILE *_fp);
    ASDLToken GetNextToken();
};
