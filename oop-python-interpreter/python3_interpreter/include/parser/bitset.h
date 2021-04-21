#pragma once
#include "common.h"
typedef uint8_t BYTE;
class Bitset{
    int bitn;
    BYTE *bitset;
public:
    Bitset(int n);
    Bitset(const int &_bitn,const BYTE *src);
    Bitset(const Bitset &src);
    Bitset(const int &_bitn,const char *src);
    void Reset();
    bool AddBit(int biti);
    bool TestBit(int biti) const;
    ~Bitset();
    std::string ToString() const;
    friend bool operator ==(const Bitset &a,const Bitset &b);
    friend Bitset &operator |=(Bitset &a,const Bitset &b);
};
