#pragma once
#include "common.h"
struct Field{
    std::string type_id;
    bool is_opt=false;
    bool is_star=false;
    std::string id;
    Field(const std::string &_type_id,const std::string &sgn,const std::string &_id):type_id(_type_id),id(_id){
        if (sgn=="?") this->is_opt=true;
        if (sgn=="*") this->is_star=true;
    }
};
struct Product{
    std::vector<Field*> fields;
    std::vector<Field*> attributes;
    Product(const std::vector<Field*> &_fields,const std::vector<Field*> &_attributes):fields(_fields),attributes(_attributes){}
};
struct Constructor{
    std::string constructor_id;
    std::vector<Field*> fields;
    Constructor(const std::string &_constructor_id,const std::vector<Field*> &_fields):constructor_id(_constructor_id),fields(_fields){}
};
struct Sum{
    std::vector<Constructor*> constructors;
    std::vector<Field*> attributes;
    Sum(const std::vector<Constructor*> &_constructors,const std::vector<Field*> &_attributes):constructors(_constructors),attributes(_attributes){}
};
struct Definition{
    std::string type_id;
    enum{product_kind,sum_kind} kind;
    union{
        Product *product;
        Sum *sum;
    };
    Definition(const std::string &_type_id,Product *_product):type_id(_type_id),kind(product_kind),product(_product){}
    Definition(const std::string &_type_id,Sum *_sum):type_id(_type_id),kind(sum_kind),sum(_sum){}
    void PrintDef(FILE *fp) const;
};
struct Module{
    std::string module_id;
    std::vector<Definition*> definitions;
    Module(const std::string &_module_id,const std::vector<Definition*> &_definitions):module_id(_module_id),definitions(_definitions){}
    void PrintDef(FILE *fp) const;
    void PrintImplement(FILE *fp) const;
};
