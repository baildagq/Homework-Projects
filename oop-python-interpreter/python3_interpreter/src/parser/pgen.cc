#include "pgen.h"
NFAGrammar::NFAGrammar(Node *n){
    this->AddLabel(ENDMARKER,"EMPTY");
    assert(n->type==MSTART);
    assert(!n->childs.empty());
    assert(n->childs[(int)n->childs.size()-1]->type==ENDMARKER);
    for (int i=0;i<(int)n->childs.size()-1;i++){
        if (n->childs[i]->type!=NEWLINE) this->CompileRULE(n->childs[i]);
    }
}
void NFAGrammar::CompileRULE(Node *n){
    assert(n->type==RULE);
    assert((int)n->childs.size()==4);
    assert(n->childs[0]->type==NAME);
    assert(n->childs[1]->type==COLON);
    assert(n->childs[2]->type==RHS);
    assert(n->childs[3]->type==NEWLINE);
    NFA nfa{NT_OFFSET+(int)this->nfas.size(),n->childs[0]->str,{}};
    this->AddLabel(NAME,nfa.name);
    auto t=this->CompileRHS(n->childs[2],&nfa);
    nfa.initial=t.first;
    nfa.accept=t.second;
    this->nfas.push_back(nfa);
}
int NFA::AddNFAState(){
    this->states.push_back({});
    return (int)this->states.size()-1;
}
int NFA::AddArc(int src,int dst,int label){
    //printf("%s %d %d %d\n",this->name.c_str(),src,dst,label);
    this->states[src].arcs.push_back({label,dst});
    return (int)this->states[src].arcs.size();
}
std::pair<int,int> NFAGrammar::CompileRHS(Node *n,NFA *nfa){
    assert(n->type==RHS);
    int initial=nfa->AddNFAState();
    int accept=nfa->AddNFAState();
    for (int i=0;i<(int)n->childs.size();i+=2){
        assert(n->childs[i]->type==ALT);
        auto t=this->CompileALT(n->childs[i],nfa);
        nfa->AddArc(initial,t.first,EMPTY);
        nfa->AddArc(t.second,accept,EMPTY);
        if (i==(int)n->childs.size()-1) break;
        assert(i+2<(int)n->childs.size());
        assert(n->childs[i+1]->type==VBAR);
    }
    return std::make_pair(initial,accept);
}
std::pair<int,int> NFAGrammar::CompileALT(Node *n,NFA *nfa){
    assert(n->type==ALT);
    assert((int)n->childs.size()>=1);
    int initial=nfa->AddNFAState();
    int accept=initial;
    for (int i=0;i<(int)n->childs.size();i++){
        auto t=this->CompileITEM(n->childs[i],nfa);
        nfa->AddArc(accept,t.first,EMPTY);
        accept=t.second;
    }
    return std::make_pair(initial,accept);
}
std::pair<int,int> NFAGrammar::CompileITEM(Node *n,NFA *nfa){
    assert(n->type==ITEM);
    assert((int)n->childs.size()>=1);
    if (n->childs[0]->type==ATOM){
        if ((int)n->childs.size()==1){
            auto t=this->CompileATOM(n->childs[0],nfa);
            return t;
        } else if ((int)n->childs.size()==2){
            assert(n->childs[1]->type==STAR||n->childs[1]->type==PLUS);
            auto t=this->CompileATOM(n->childs[0],nfa);
            nfa->AddArc(t.second,t.first,EMPTY);
            if (n->childs[1]->type==STAR) t.second=t.first;
            return t;
        } else{
            assert((int)n->childs.size()<=2);
            assert(false);
        }
    } else if (n->childs[0]->type==LSQB){
        assert((int)n->childs.size()==3);
        assert(n->childs[0]->type==LSQB);
        assert(n->childs[1]->type==RHS);
        assert(n->childs[2]->type==RSQB);
        auto t=this->CompileRHS(n->childs[1],nfa);
        int initial=nfa->AddNFAState();
        int accept=nfa->AddNFAState();
        nfa->AddArc(initial,accept,EMPTY);
        nfa->AddArc(initial,t.first,EMPTY);
        nfa->AddArc(t.second,accept,EMPTY);
        return std::make_pair(initial,accept);
    } else{
        assert(n->childs[0]->type==ATOM||n->childs[0]->type==LSQB);
        assert(false);
    }
}
std::pair<int,int> NFAGrammar::CompileATOM(Node *n,NFA *nfa){
    assert(n->type==ATOM);
    assert((int)n->childs.size()>=1);
    if (n->childs[0]->type==LPAR){
        assert((int)n->childs.size()==3);
        assert(n->childs[1]->type==RHS);
        assert(n->childs[2]->type==RPAR);
        auto t=this->CompileRHS(n->childs[1],nfa);
        return t;
    } else if (n->childs[0]->type==NAME||n->childs[0]->type==STRING){
        assert((int)n->childs.size()==1);
        int initial=nfa->AddNFAState();
        int accept=nfa->AddNFAState();
        nfa->AddArc(initial,accept,this->AddLabel(n->childs[0]->type,n->childs[0]->str));
        return std::make_pair(initial,accept);
    } else{
        assert(false);
    }
}
int NFAGrammar::AddLabel(int type,const std::string &str){
    for (int i=0;i<(int)this->labels.size();i++)
        if (this->labels[i].type==type&&this->labels[i].str==str) return i;
    this->labels.push_back({type,str});
    return (int)this->labels.size()-1;
}
void NFA::AddClosure(Bitset *bitset,int state){
    if (!bitset->AddBit(state)) return;
    for (const auto &arc:this->states[state].arcs)
        if (arc.label==EMPTY) this->AddClosure(bitset,arc.dst);
}
Bitset *NFA::Trans(const Bitset *bitset0,int label){
    int nbits=(int)this->states.size();
    Bitset *bitset=new Bitset(nbits);
    for (int i=0;i<nbits;i++) if (bitset0->TestBit(i))
        for (const auto &arc:this->states[i].arcs) if (arc.label==label){
            this->AddClosure(bitset,arc.dst);
        }
    return bitset;
}
bool SameDFAState(DFA *dfa,bool is_accept[],int i,int j){
    if (is_accept[i]!=is_accept[j]) return false;
    if (dfa->states[i].arcs.size()!=dfa->states[j].arcs.size()) return false;
    for (int k=0;k<(int)dfa->states[i].arcs.size();k++)
        if (dfa->states[i].arcs[k].label!=dfa->states[j].arcs[k].label||dfa->states[i].arcs[k].dst!=dfa->states[j].arcs[k].dst) return false;
    return true;
}
void RemoveDFAStates(DFA *dfa,int src,int dst){
    //printf("%d To %d\n",src,dst);
    for (auto &state:dfa->states) for (auto &arc:state.arcs) if (arc.dst==src) arc.dst=dst;
}
void SimplifyDFA(DFA *dfa,bool is_accept[]){
    int n=(int)dfa->states.size();
    bool *is_deleted=new bool[n];
    memset(is_deleted,0,sizeof(*is_deleted)*n);
    for (;;){
        for (int i=0;i<n;i++) std::sort(dfa->states[i].arcs.begin(),dfa->states[i].arcs.end(),[](const Arc &i,const Arc &j){return i.label!=j.label?i.label<j.label:i.dst<j.dst;});
        bool flag=true;
        for (int i=0;flag&&i<n;i++) if (!is_deleted[i]) for (int j=0;flag&&j<n;j++) if (!is_deleted[j]){
            if (i!=j&&i!=dfa->initial&&SameDFAState(dfa,is_accept,i,j)){
                RemoveDFAStates(dfa,i,j);
                is_deleted[i]=true;
                flag=false;
            }
        }
        if (flag) break;
    }
    int initial0=dfa->initial;
    assert(!is_deleted[initial0]);
    dfa->initial=0;
    for (int i=0;i<initial0;i++) if (!is_deleted[i]) dfa->initial++;
    std::vector<DFAState> states;
    for (int i=0;i<n;i++) if (is_deleted[i]) states.push_back(dfa->states[i]);
    //printf("before:%d ",(int)dfa->states.size());
    dfa->states.clear();
    dfa->states=states;
    //printf("after:%d\n",(int)dfa->states.size());
}
void NFA::MakeDFA(DFA *dfa){
    int nbits=(int)this->states.size();
    //printf("nbits:%d %s initial:%d accept:%d\n",nbits,dfa->name.c_str(),this->initial,this->accept);
    std::vector<Bitset*> bitsets;
    Bitset *bitset0=new Bitset(nbits);
    this->AddClosure(bitset0,this->initial);
    bitsets.push_back(bitset0);
    dfa->states.push_back({{}});
    for (int i=0;i<(int)bitsets.size();i++){
        //printf("%s\n",bitsets[i]->ToString().c_str());
        for (int j=0;j<nbits;j++) if (bitsets[i]->TestBit(j)){
            for (const auto &arc:this->states[j].arcs) if (arc.label!=EMPTY){
                //printf("%d %d %d\n",j,arc.label,arc.dst);
                Bitset *bitset=this->Trans(bitsets[i],arc.label);
                int index=-1;
                for (int j=0;j<(int)bitsets.size();j++) if (*bitset==*bitsets[j]) index=j;
                if (index<0){
                    index=(int)bitsets.size();
                    bitsets.push_back(bitset);
                    dfa->states.push_back({{}});
                } else delete bitset;
                dfa->states[i].arcs.push_back({arc.label,index});
            }
        }
    }
    bool *is_accept=new bool[(int)bitsets.size()];
    for (int i=0;i<(int)bitsets.size();i++) is_accept[i]=bitsets[i]->TestBit(this->accept);
    for (int i=0;i<(int)dfa->states.size();i++) if (is_accept[i]) dfa->states[i].arcs.push_back({EMPTY,i});
    //SimplifyDFA(dfa,is_accept);
    delete[] is_accept;
    for (auto bitset:bitsets) delete bitset;
}
Grammar *pgen(Node *n){
    NFAGrammar nfag(n);
    auto *g=new Grammar{nfag.labels,{}};
    for (int i=0;i<(int)nfag.nfas.size();i++){
        DFA dfa{nfag.nfas[i].type,nfag.nfas[i].name,0,{},(int)nfag.labels.size()};
        nfag.nfas[i].MakeDFA(&dfa);
        g->dfas.push_back(dfa);
    } 
    //puts("nfa");
    //for (int i=0;i<(int)g->dfas.size();i++) printf("%d %s\n",g->dfas[i].type,g->dfas[i].name.c_str());
    g->TranslateLabels();
    //puts("translate");
    g->CalcFirstSet();
    return g;
}

