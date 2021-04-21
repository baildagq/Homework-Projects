#include "asdl_tokenizer.h"
#include "asdl_token.h"
ASDLTokenizer::ASDLTokenizer(FILE *_fp):fp(_fp){
    this->buf=new char[BUFSIZE];
    this->cur=this->buf;
    *this->cur='\0';
}
char ASDLTokenizer::NextChar(){
    if (*cur=='\0'){
        if (fgets(this->buf,BUFSIZE,this->fp)==nullptr) return EOF;
        this->cur=this->buf;
        this->lineno++,this->col_offset=0;
    }
    this->col_offset++;
    return *cur++;
}
void ASDLTokenizer::BackChar(char c){
    assert(this->cur-this->buf>0);
    (*--cur)=c;
    this->col_offset--;
}
inline bool Isalpha(char c){
    return 'a'<=c&&c<='z';
}
inline bool IsAlpha(char c){
    return 'A'<=c&&c<='Z';
}
inline bool IsDigit(char c){
    return '0'<=c&&c<='9';
}
ASDLToken ASDLTokenizer::GetNextToken(){
    assert(!this->done);
    for (;;){
        char c=this->NextChar();
        if (c==EOF){
            this->done=true;
            return {-1,"",this->lineno,this->col_offset};
        }
        std::string str="";
        if (c=='-'){
            c=this->NextChar();
            assert(c=='-');
            for (;;){
                c=this->NextChar();
                if (c=='\n') break;
            }
        } else if (Isalpha(c)||IsAlpha(c)||c=='_'){
            str.push_back(c);
            for (;;){
                c=this->NextChar();
                if (Isalpha(c)||IsAlpha(c)||c=='_'||IsDigit(c)){
                    str.push_back(c);
                } else{
                    this->BackChar(c);
                    return {IsAlpha(str[0])?ConstructorID:TypeID,str,this->lineno,this->col_offset};
                }
            }
        } else{
            str.push_back(c);
            int index=ASDLTokenMap(str);
            if (index>=0) return {index,str,this->lineno,this->col_offset};
        }
    }
}
