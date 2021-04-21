#include "grammar.h"
#include "metagrammar.h"
#include "token.h"
#include "bitset.h"

std::vector<Label> labels={
    {ENDMARKER,"EMPTY"},
    {MSTART,""},
    {RULE,""},
    {NEWLINE,""},
    {ENDMARKER,""},
    {NAME,""},
    {COLON,""},
    {RHS,""},
    {ALT,""},
    {VBAR,""},
    {ITEM,""},
    {LSQB,""},
    {RSQB,""},
    {ATOM,""},
    {STAR,""},
    {STRING,""},
    {LPAR,""},
    {RPAR,""},
    {PLUS,""},
};
int get_label_index(int type){
    for (int i=1;i<(int)labels.size();i++)
        if (labels[i].type==type) return i;
    assert(false);
}
Bitset get_firstset_bitset(std::vector<int> types){
    Bitset bitset((int)labels.size());
    for (const auto &type:types){
        bitset.AddBit(get_label_index(type));
    }
    return bitset;
}

std::vector<Arc> arcs_0_0={{get_label_index(NEWLINE),0},{get_label_index(RULE),0},{get_label_index(ENDMARKER),1}};
std::vector<Arc> arcs_0_1={{EMPTY,1}};
std::vector<DFAState> states_0={arcs_0_0,arcs_0_1};
DFA dfa_0={MSTART,"MSTART",0,states_0,get_firstset_bitset({NEWLINE,NAME,ENDMARKER})};

std::vector<Arc> arcs_1_0={{get_label_index(NAME),1}};
std::vector<Arc> arcs_1_1={{get_label_index(COLON),2}};
std::vector<Arc> arcs_1_2={{get_label_index(RHS),3}};
std::vector<Arc> arcs_1_3={{get_label_index(NEWLINE),4}};
std::vector<Arc> arcs_1_4={{EMPTY,4}};
std::vector<DFAState> states_1={arcs_1_0,arcs_1_1,arcs_1_2,arcs_1_3,arcs_1_4};
DFA dfa_1={RULE,"RULE",0,states_1,get_firstset_bitset({NAME})};

std::vector<Arc> arcs_2_0={{get_label_index(ALT),1}};
std::vector<Arc> arcs_2_1={{get_label_index(VBAR),2},{EMPTY,1}};
std::vector<Arc> arcs_2_2={{get_label_index(ALT),1}};
std::vector<DFAState> states_2={arcs_2_0,arcs_2_1,arcs_2_2};
DFA dfa_2={RHS,"RHS",0,states_2,get_firstset_bitset({LPAR,LSQB,NAME,STRING})};

std::vector<Arc> arcs_3_0={{get_label_index(ITEM),1}};
std::vector<Arc> arcs_3_1={{get_label_index(ITEM),1},{EMPTY,1}};
std::vector<DFAState> states_3={arcs_3_0,arcs_3_1};
DFA dfa_3={ALT,"ALT",0,states_3,get_firstset_bitset({LPAR,LSQB,NAME,STRING})};

std::vector<Arc> arcs_4_0={{get_label_index(ATOM),1},{get_label_index(LSQB),2}};
std::vector<Arc> arcs_4_1={{get_label_index(STAR),4},{get_label_index(PLUS),4},{EMPTY,1}};
std::vector<Arc> arcs_4_2={{get_label_index(RHS),3}};
std::vector<Arc> arcs_4_3={{get_label_index(RSQB),4}};
std::vector<Arc> arcs_4_4={{EMPTY,4}};
std::vector<DFAState> states_4={arcs_4_0,arcs_4_1,arcs_4_2,arcs_4_3,arcs_4_4};
DFA dfa_4={ITEM,"ITEM",0,states_4,get_firstset_bitset({LPAR,LSQB,NAME,STRING})};

std::vector<Arc> arcs_5_0={{get_label_index(LPAR),1},{get_label_index(NAME),3},{get_label_index(STRING),3}};
std::vector<Arc> arcs_5_1={{get_label_index(RHS),2}};
std::vector<Arc> arcs_5_2={{get_label_index(RPAR),3}};
std::vector<Arc> arcs_5_3={{EMPTY,3}};
std::vector<DFAState> states_5={arcs_5_0,arcs_5_1,arcs_5_2,arcs_5_3};
DFA dfa_5={ATOM,"ATOM",0,states_5,get_firstset_bitset({LPAR,NAME,STRING})};

std::vector<DFA> dfas={
    dfa_0,
    dfa_1,
    dfa_2,
    dfa_3,
    dfa_4,
    dfa_5,
};

Grammar meta_grammar(labels,dfas);

Grammar *MetaGrammar(){
    return &meta_grammar;
}
