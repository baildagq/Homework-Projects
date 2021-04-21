#pragma once
#include "common.h"
enum{
    ConstructorID,
    TypeID,
    EQUAL,
    COMMA,
    STAR,
    QUESTION,
    VBAR,
    LPAR,
    RPAR,
    LBRACE,
    RBRACE,
};
int ASDLTokenMap(std::string str);
