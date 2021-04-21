#pragma once

#include "common.h"
#include "token.h"

struct Token{
    int type;
    std::string str;
    int lineno;
    int col_offset;
    int level;
    Token(int _type, const std::string &_str, int _lineno, int _col_offset): type(_type), str(_str), lineno(_lineno), col_offset(_col_offset){};
};

class Tokenizer
{
    static const int BUFSIZE=1000;
    static const int MAXINDENT=100;
    static const int TABSIZE=8;
    FILE *fp;
    int lineno;
    int col_offset;
    char *buf = nullptr;
    char *cur = nullptr;
    bool done = false;
    char NextChar();
    void BackChar(char c);

    int tabsize = TABSIZE;
    int indent = 0; /* Current indentation index */
    int indstack[MAXINDENT];/* Stack of indents */
    int pendin = 0;
    int atbol = 1;/* Nonzero if at begin of new line */
    int level = 0;
public:
    Tokenizer(FILE *_fp): fp(_fp){
        buf = new char[BUFSIZE];
        cur = buf;
        *cur = '\0';

        indstack[0] = 1;
    }
    Token GetNextToken();
};


