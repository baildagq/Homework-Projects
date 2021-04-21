#include "common.h"
#include "bitset.h"
Bitset::Bitset(int n){
    bitn=(n+7)>>3;
    bitset=new BYTE[bitn];
    memset(bitset,0,sizeof(*bitset)*bitn);
}
Bitset::Bitset(const int &_bitn,const BYTE *src):bitn(_bitn){
    bitset=new BYTE[bitn];
    memcpy(bitset,src,sizeof(*bitset)*bitn);
}
Bitset::Bitset(const Bitset &src):Bitset(src.bitn,src.bitset){}
Bitset::Bitset(const int &_bitn,const char *src):bitn(_bitn){
    bitset=new BYTE[bitn];
    memcpy(bitset,src,sizeof(*bitset)*bitn);
}
Bitset::~Bitset(){
    delete[] bitset;
}
void Bitset::Reset(){
    memset(bitset,0,sizeof(*bitset)*bitn);
}
bool Bitset::AddBit(int biti){
    int pos=biti>>3,mask=1<<(biti&7);
    if (bitset[pos]&mask) return false;
    bitset[pos]|=mask;
    return true;
}
bool Bitset::TestBit(int biti) const{
    int pos=biti>>3,mask=1<<(biti&7);
    assert(pos<this->bitn);
    return bool(bitset[pos]&mask);
}
bool operator ==(const Bitset &a,const Bitset &b){
    if (a.bitn!=b.bitn) return false;
    for (int i=0;i<a.bitn;i++) if (a.bitset[i]!=b.bitset[i]) return false;
    return true;
}
Bitset &operator |=(Bitset &a,const Bitset &b){
    for (int i=0;i<a.bitn&&i<b.bitn;i++) a.bitset[i]|=b.bitset[i];
    return a;
}
std::string Bitset::ToString() const{
    std::string str;
    char buf[5];
    for (int i=0;i<this->bitn;i++){
        sprintf(buf,"\\%03o",this->bitset[i]);
        buf[4]='\0';
        str+=buf;
    }
    return str;
}
