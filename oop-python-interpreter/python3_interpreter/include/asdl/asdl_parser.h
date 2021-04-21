#pragma once
#include "common.h"
#include "asdl_tokenizer.h"
#include "asdl_node.h"
class ASDLTokenizerIterator{
    ASDLTokenizer *tokenizer=nullptr;
    ASDLToken token{EOF,"",0,0};
public:
    ASDLTokenizerIterator();
    ~ASDLTokenizerIterator();
    void Reset(FILE *fp);
    const ASDLToken &operator *();
    const ASDLToken *operator ->();
    void operator ++(int);
};
class ASDLParser{
    ASDLTokenizerIterator iter;
    Field *ParseField();
    std::vector<Field*> ParseFields();
    Constructor *ParseConstructor();
    Product *ParseProduct();
    Sum *ParseSum();
    Definition *ParseDefinition();
    Module *ParseModule();
public:
    ASDLParser();
    Module *ParseFile(FILE *fp);
};
