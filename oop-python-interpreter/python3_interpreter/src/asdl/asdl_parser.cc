#include "asdl_token.h"
#include "asdl_parser.h"
ASDLTokenizerIterator::ASDLTokenizerIterator(){}
ASDLTokenizerIterator::~ASDLTokenizerIterator(){
    if (this->tokenizer!=nullptr) delete this->tokenizer;
}
void ASDLTokenizerIterator::Reset(FILE *fp){
    if (this->tokenizer!=nullptr) delete this->tokenizer;
    this->tokenizer=new ASDLTokenizer(fp);
    token=this->tokenizer->GetNextToken();
}
const ASDLToken &ASDLTokenizerIterator::operator *(){
    return this->token;
}
const ASDLToken *ASDLTokenizerIterator::operator ->(){
    return &this->token;
}
void ASDLTokenizerIterator::operator ++(int){
    //printf("%s->",this->token.str.c_str());
    assert(this->token.type!=EOF);
    this->token=this->tokenizer->GetNextToken();
    //printf("%s\n",this->token.str.c_str());
}
ASDLParser::ASDLParser(){}
Field *ASDLParser::ParseField(){
    assert(this->iter->type==TypeID);
    std::string type_id=this->iter->str;
    this->iter++;
    if (this->iter->type==QUESTION||this->iter->type==STAR){
        std::string sgn=this->iter->str;
        this->iter++;
        if (this->iter->type==TypeID){
            auto *field=new Field(type_id,sgn,this->iter->str);
            this->iter++;
            return field;
        } else{
            auto *field=new Field(type_id,sgn,"");
            return field;
        }
    } else if (this->iter->type==TypeID){
        auto *field=new Field(type_id,"",this->iter->str);
        this->iter++;
        return field;
    } else{
        auto *field=new Field(type_id,"","");
        return field;
    }
}
std::vector<Field*> ASDLParser::ParseFields(){
    assert(this->iter->type==LPAR);
    this->iter++;
    std::vector<Field*> fields{this->ParseField()};
    for (;;){
        if (this->iter->type!=COMMA) break;
        this->iter++;
        fields.push_back(this->ParseField());
    }
    assert(this->iter->type==RPAR);
    this->iter++;
    return fields;
}
Product *ASDLParser::ParseProduct(){
    auto *product=new Product(this->ParseFields(),{});
    if (this->iter->str=="attributes"){
        this->iter++;
        product->attributes=this->ParseFields();
    }
    return product;
}
Constructor *ASDLParser::ParseConstructor(){
    assert(this->iter->type==ConstructorID);
    auto *constructor=new Constructor(this->iter->str,{});
    this->iter++;
    if (this->iter->type==LPAR){
        constructor->fields=this->ParseFields();
    }
    return constructor;
}
Sum *ASDLParser::ParseSum(){
    auto *sum=new Sum{{},{}};
    sum->constructors.push_back(this->ParseConstructor());
    for (;;){
        if (this->iter->type!=VBAR) break;
        this->iter++;
        sum->constructors.push_back(this->ParseConstructor());
    }
    if (this->iter->str=="attributes"){
        this->iter++;
        sum->attributes=this->ParseFields();
    }
    return sum;
}
Definition *ASDLParser::ParseDefinition(){
    assert(this->iter->type==TypeID);
    std::string type_id=this->iter->str;
    this->iter++;
    assert(this->iter->type==EQUAL);
    this->iter++;
    if (this->iter->type==LPAR){
        auto *def=new Definition{type_id,this->ParseProduct()};
        return def;
    } else{
        auto *def=new Definition{type_id,this->ParseSum()};
        return def;
    }
}
Module *ASDLParser::ParseModule(){
    assert(this->iter->type==TypeID&&this->iter->str=="module");
    iter++;
    assert(this->iter->type==ConstructorID);
    Module *mod=new Module(this->iter->str,{});
    this->iter++;
    assert(this->iter->type==LBRACE);
    this->iter++;
    for (;;){
        if (this->iter->type!=TypeID) break;
        mod->definitions.push_back(this->ParseDefinition());
    }
    assert(this->iter->type==RBRACE);
    this->iter++;
    assert(this->iter->type==EOF);
    return mod;
}
Module *ASDLParser::ParseFile(FILE *fp){
    this->iter.Reset(fp);
    Module *mod=this->ParseModule();
    return mod;
}
