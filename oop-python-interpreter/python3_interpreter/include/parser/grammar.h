#pragma once
#include "common.h"
#include "bitset.h"
struct Label{
    int type;
    std::string str;
    Label(int _type,const std::string &_str):type(_type),str(_str){}
};
struct Arc{
    int label;
    int dst;
    Arc(const short &_label,const short &_dst):label(_label),dst(_dst){}
};
struct DFAState{
    std::vector<Arc> arcs;
    DFAState(const std::vector<Arc> &_arcs):arcs(_arcs){}
    int lower,upper;
    int *accel=nullptr;
    bool accept=false;
    ~DFAState(){if (accel) delete[] accel;}
};
struct DFA{
    int type;
    std::string name;
    int initial;
    std::vector<DFAState> states;
    Bitset first;
    DFA(const int &_type,const std::string &_name,const int &_initial,const std::vector<DFAState> &_states,const Bitset &_first):type(_type),name(_name),initial(_initial),states(_states),first(_first){}
};
class Grammar{
public:
    std::vector<Label> labels;
    std::vector<DFA> dfas;
    bool is_accel=false;
    Grammar(const std::vector<Label> &_labels,const std::vector<DFA> &_dfas):labels(_labels),dfas(_dfas){}
    void CalcFirstSet(int status[],int i);
    void CalcFirstSet();
    void AddAccelerator();
    void TranslateLabels(Label &label);
    void TranslateLabels();
    int FindDFAIndex(int type) const;
    const DFA &FindDFA(int type) const;
    void PrintDFAs(FILE *fp) const;
    void PrintLabels(FILE *fp) const;
    void PrintGrammar(FILE *fp) const;
    void PrintNonterminals(FILE *fp) const;
};
const int EMPTY=0;
