#include "asdl_token.h"
int ASDLTokenMap(std::string str){
    if (str=="=") return EQUAL;
    if (str==",") return COMMA;
    if (str=="|") return VBAR;
    if (str=="*") return STAR;
    if (str=="?") return QUESTION;
    if (str=="(") return LPAR;
    if (str==")") return RPAR;
    if (str=="{") return LBRACE;
    if (str=="}") return RBRACE;
    return -1;
}
