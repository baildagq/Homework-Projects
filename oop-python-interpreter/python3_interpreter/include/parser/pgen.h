#pragma once
#include "common.h"
#include "grammar.h"
#include "node.h"
#include "metagrammar.h"
struct NFAState{
    std::vector<Arc> arcs;
};
class NFA{
public:
    int type;
    std::string name;
    int initial=-1,accept=-1;
    std::vector<NFAState> states;
    NFA(int _type,const std::string _name,const std::vector<NFAState> _states):type(_type),name(_name),states(_states){}
    int AddNFAState();
    int AddArc(int src,int dst,int label);
    void MakeDFA(DFA *dfa);
    void AddClosure(Bitset *bitset0,int index);
    Bitset *Trans(const Bitset *bitset,int label);
};
class NFAGrammar{
    void CompileRULE(Node *n);
    std::pair<int,int> CompileRHS(Node *n,NFA *nfa);
    std::pair<int,int> CompileALT(Node *n,NFA *nfa);
    std::pair<int,int> CompileITEM(Node *n,NFA *nfa);
    std::pair<int,int> CompileATOM(Node *n,NFA *nfa);
public:
    std::vector<Label> labels;
    std::vector<NFA> nfas;
    NFAGrammar(Node *n);
    int AddLabel(int type,const std::string &str);
};
Grammar *pgen(Node *n);
