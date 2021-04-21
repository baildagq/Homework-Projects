#include "tokenizer.h"

char Tokenizer::NextChar(){
    if (*cur=='\0'){
        if (fgets(this->buf,BUFSIZE,this->fp)==nullptr) return EOF;
        this->cur=this->buf;
        this->lineno++,this->col_offset=0;
    }
    this->col_offset++;
    return *cur++;
}
void Tokenizer::BackChar(char c){
    assert(cur-buf>0);
    (*--cur)=c;
    this->col_offset--;
}

bool is_digit(const char x)
{
    return '0'<=x&&x<='9';
}
bool is_alpha_or_underline(const char x)
{
    if( x == '_')
        return true;

    return ('a'<=x&&x<='z')||('A'<=x&&x<='Z');
}

Token Tokenizer::GetNextToken(){
    assert(!this->done);
    for (;;){
        char c=this->NextChar();
        int blankline = 0;
        if (c==EOF){
            while(indent-- != 0){
                BackChar(c);
                return {DEDENT, "",lineno, col_offset};
            }
            this->done=true;
            return {ENDMARKER,"",this->lineno,this->col_offset};
        }
        if(this->atbol){
            this->atbol = 0;
            for(;;){
                if(c == ' '){
                    this->col_offset ++;
                    c = NextChar();
                }
                else if(c == '\t'){
                    this->col_offset = (this->col_offset/this->tabsize + 1)*this->tabsize;
                    c = NextChar();
                }
                else{
                    break;
                }
            }
            if(c == '#' || c == '\n' || c == '\r'){
                blankline = 1;
            }
            if(!blankline&&this->level==0){
                if(this->col_offset == this->indstack[this->indent]){}
                else if(this->col_offset > this->indstack[this->indent]){
                    this->pendin ++;
                    this->indstack[++this->indent] = this->col_offset;
                }
                else{
                    while(this->indent>0&&this->col_offset < this->indstack[this->indent]){
                        this->pendin --;
                        this->indent --;
                    }
                }
            }
        }
        if(this->pendin!=0){
            if(this->pendin < 0){
                this->pendin ++;
                BackChar(c);
                return {DEDENT,"",this->lineno,this->col_offset};
            }
            else{
                this->pendin --;
                BackChar(c);
                return {INDENT,"",this->lineno,this->col_offset};
            }
        }
        std::string str="";

        again:

        while(c == ' '||c == '\t'){
            c = NextChar();
        }

        if (c=='#'){
            while(c!=EOF&&c!='\n'&&c!='\r'){
                c = NextChar();
            }
            if(blankline){
                if(c == '\r') {
                    c = NextChar();
                    assert(c == '\n');
                }
                this->atbol = 1;
            }
            else{
                if(c == '\n'){
                    BackChar(c);
                }
            }

        } else if (c=='\n' || c == '\r'){
            if(c == '\r'){
                c = NextChar();
                assert(c == '\n');
            }
            this->atbol = 1;
            if(blankline||level){
                continue;
            }
            str.push_back(c);
            return {NEWLINE,str,this->lineno,this->col_offset};
        } else if (is_alpha_or_underline(c)){
            str.push_back(c);
            for (;;){
                c=this->NextChar();
                if (is_alpha_or_underline(c)||is_digit(c)){
                    str.push_back(c);
                } else{
                    this->BackChar(c);
                    return {NAME,str,this->lineno,this->col_offset};
                }
            }
        } else if(is_digit(c)){
            str.push_back(c);
            while(1){
                c = NextChar();
                if(c == '_'){
                    c = NextChar();
                }
                if(is_digit(c)){
                    str.push_back(c);
                }
                else{
                    BackChar(c);
                    break;
                }
            }
            return {NUMBER,str,this->lineno,this->col_offset};
        } else if (c=='\''||c=='"'){
            str.push_back(c);
            char quote = c;
            int quote_size = 1;
            int end_quote_size = 0;

            c = NextChar();
            if(c == quote){
                str.push_back(c);
                c = NextChar();
                if(c == quote){
                    quote_size = 3;
                    str.push_back(c);
                }
                else{
                    end_quote_size = 1;
                    BackChar(c);
                }
            }
            if(c!=quote){
                BackChar(c);
            }

            while(end_quote_size!=quote_size){
                c = NextChar();
                assert(c!=EOF);
                if(c == quote){
                    end_quote_size ++;
                    str.push_back(c);
                }
                else{
                    end_quote_size = 0;
                    if(c == '\\'){
                        c = NextChar();
                        assert(c!=EOF);
                    }
                    str.push_back(c);
                }
            }
            return {STRING,str,this->lineno,this->col_offset};
        } else if(c == '\\'){
            c = NextChar();
            assert(c!='\n');
            goto again;
        } else if (c=='='){
            str.push_back(c);
            c = NextChar();
            switch(c){
                case '=':
                    str.push_back(c);
                    return {EQEQUAL,str,this->lineno,this->col_offset};
                default:
                    BackChar(c);
                    return {EQUAL,str,this->lineno,this->col_offset};
            }
        } else if (c=='!'){
            str.push_back(c);
            c = NextChar();
            switch(c){
                case '=':
                    str.push_back(c);
                    return {NOTEQUAL,str,this->lineno,this->col_offset};
                default:
                    BackChar(c);
                    break;
            }
        } else if (c=='<'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '>':
                    str.push_back(c);
                    return {NOTEQUAL,str,this->lineno,this->col_offset};
                case '=':
                    str.push_back(c);
                    return {LESSEQUAL,str,this->lineno,this->col_offset};
                case '<':
                    str.push_back(c);
                    c = NextChar();
                    switch (c){
                        case '=':
                            str.push_back(c);
                            return {LEFTSHIFTEQUAL,str,this->lineno,this->col_offset};
                        default:
                            BackChar(c);
                            return {LEFTSHIFT,str,this->lineno,this->col_offset};
                    }
                default:
                    BackChar(c);
                    return {LESS,str,this->lineno,this->col_offset};
            }
        } else if (c=='>'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '=':
                    str.push_back(c);
                    return {GREATEREQUAL,str,this->lineno,this->col_offset};
                case '>':
                    str.push_back(c);
                    c = NextChar();
                    switch (c){
                        case '=':
                            str.push_back(c);
                            return {RIGHTSHIFTEQUAL,str,this->lineno,this->col_offset};
                        default:
                            BackChar(c);
                            return {RIGHTSHIFT,str,this->lineno,this->col_offset};
                    }
                default:
                    BackChar(c);
                    return {GREATER,str,this->lineno,this->col_offset};
            }
        } else if (c=='+'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '=':
                    str.push_back(c);
                    return {PLUSEQUAL,str,this->lineno,this->col_offset};
                default:
                    BackChar(c);
                    return {PLUS,str,this->lineno,this->col_offset};
            }
        } else if (c=='-'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '=':
                    str.push_back(c);
                    return {MINEQUAL,str,this->lineno,this->col_offset};
                case '>':
                    str.push_back(c);
                    return {RARROW,str,this->lineno,this->col_offset};
                default:
                    BackChar(c);
                    return {MINUS,str,this->lineno,this->col_offset};
            }
        } else if (c=='*'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '=':
                    str.push_back(c);
                    return {STAREQUAL,str,this->lineno,this->col_offset};
                case '*':
                    str.push_back(c);
                    c = NextChar();
                    switch (c){
                        case '=':
                            str.push_back(c);
                            return {DOUBLESTAREQUAL,str,this->lineno,this->col_offset};
                        default:
                            BackChar(c);
                            return {DOUBLESTAR,str,this->lineno,this->col_offset};
                    }
                default:
                    BackChar(c);
                    return {STAR,str,this->lineno,this->col_offset};
            }
        } else if (c=='/'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '=':
                    str.push_back(c);
                    return {SLASHEQUAL,str,this->lineno,this->col_offset};
                case '/':
                    str.push_back(c);
                    c = NextChar();
                    switch (c){
                        case '=':
                            str.push_back(c);
                            return {DOUBLESLASHEQUAL,str,this->lineno,this->col_offset};
                        default:
                            BackChar(c);
                            return {DOUBLESLASH,str,this->lineno,this->col_offset};
                    }
                default:
                    BackChar(c);
                    return {SLASH,str,this->lineno,this->col_offset};
            }
        } else if (c=='|'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '=':
                    str.push_back(c);
                    return {VBAREQUAL,str,this->lineno,this->col_offset};
                default:
                    BackChar(c);
                    return {VBAR,str,this->lineno,this->col_offset};
            }
        } else if (c=='%'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '=':
                    str.push_back(c);
                    return {PERCENTEQUAL,str,this->lineno,this->col_offset};
                default:
                    BackChar(c);
                    return {PERCENT,str,this->lineno,this->col_offset};
            }
        } else if (c=='&'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '=':
                    str.push_back(c);
                    return {AMPEREQUAL,str,this->lineno,this->col_offset};
                default:
                    BackChar(c);
                    return {AMPER,str,this->lineno,this->col_offset};
            }
        } else if (c=='^'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '=':
                    str.push_back(c);
                    return {CIRCUMFLEXEQUAL,str,this->lineno,this->col_offset};
                default:
                    BackChar(c);
                    return {CIRCUMFLEX,str,this->lineno,this->col_offset};
            }
        } else if (c=='@'){
            str.push_back(c);
            c = NextChar();
            switch (c){
                case '=':
                    str.push_back(c);
                    return {ATEQUAL,str,this->lineno,this->col_offset};
                default:
                    BackChar(c);
                    return {AT,str,this->lineno,this->col_offset};
            }
        } else if (c == '.'){
            str.push_back(c);
            c = NextChar();
            if(c == '.'){
                char c2 = NextChar();
                if(c2 == '.'){
                    str.push_back(c);
                    str.push_back(c2);
                    return {ELLIPSIS,str,this->lineno,this->col_offset};
                } else{
                    BackChar(c2);
                    BackChar(c);
                }
            }
            BackChar(c);
            return {DOT,str,this->lineno,this->col_offset};
        }
        else if (c==':'){
            str.push_back(c);
            return {COLON,str,this->lineno,this->col_offset};
        } else if (c=='('){
            level ++;
            str.push_back(c);
            return {LPAR,str,this->lineno,this->col_offset};
        } else if (c==')'){
            level --;
            str.push_back(c);
            return {RPAR,str,this->lineno,this->col_offset};
        } else if (c=='['){
            level ++;
            str.push_back(c);
            return {LSQB,str,this->lineno,this->col_offset};
        } else if (c==']'){
            level --;
            str.push_back(c);
            return {RSQB,str,this->lineno,this->col_offset};
        } else if (c=='{'){
            level ++;
            str.push_back(c);
            return {LBRACE,str,this->lineno,this->col_offset};
        } else if (c=='}'){
            level --;
            str.push_back(c);
            return {RBRACE,str,this->lineno,this->col_offset};
        }else if (c==',') {
            str.push_back(c);
            return {COMMA, str, this->lineno, this->col_offset};
        } else if (c==';'){
            str.push_back(c);
            return {SEMI,str,this->lineno,this->col_offset};
        } else if (c=='~'){
            str.push_back(c);
            return {TILDE,str,this->lineno,this->col_offset};
        }
    }
}
