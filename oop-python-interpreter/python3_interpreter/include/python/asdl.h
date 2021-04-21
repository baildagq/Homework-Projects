#pragma once
#include "common.h"
#include "object.h"
typedef Object identifier_ty;
typedef Object string_ty;
typedef Object object_ty;
typedef Object singleton_ty;
//typedef Object bytes;
//typedef Object singleton;
//typedef Object constant;
template <typename T>
class asdl_seq{
    std::vector<T> elements;
public:
    asdl_seq(const std::vector<T> &_elements):elements(_elements){}
    void Reverse(){std::reverse(elements.begin(),elements.end());}
    void PushBack(const T &x){this->elements.push_back(x);}
    void PushFront(const T &x){this->Reverse();this->PushBack(x);this->Reverse();}
    std::vector<T> ToVector(){return this->elements;}
    int Size(){return (int)this->elements.size();}
    T Get(int i){return this->elements[i];}
};
