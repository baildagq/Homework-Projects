#include "parser.h"
#include "node.h"
#include "tokenizer.h"
#include "token.h"
void Parser::GarbageCollection(Node *p){
    if (p==nullptr) return;
    for (auto q:p->childs) this->GarbageCollection(q);
    delete p;
}
void Parser::Reset(){
    this->grammar->AddAccelerator();
    this->GarbageCollection(this->tree);
    this->tree=new Node(start,"",{},0,0);
    const auto &dfa=this->grammar->FindDFA(this->start);
    this->stack.push({dfa.initial,dfa,this->tree});
}
Parser::~Parser(){
    this->GarbageCollection(this->tree);
}
int Parser::classify(const Token &token) const{
    const auto &labels=this->grammar->labels;
    if (token.type==NAME){
        for (int i=0;i<(int)labels.size();i++){
            if (labels[i].type==NAME&&!labels[i].str.empty()&&token.str==labels[i].str){
                return i;
            }
        }
    }
    for (int i=0;i<(int)labels.size();i++){
        if (labels[i].type==token.type&&labels[i].str.empty()){
            return i;
        }
    }
    assert(false);
}
void Parser::AddToken(const Token &token){
    int label_index=this->classify(token);
    assert(label_index>=0);
    for (;;){
        assert(!this->stack.empty());
        const auto &dfa=this->stack.top().dfa;
        const auto &state=dfa.states[this->stack.top().state];
        //printf("%d cur dfa:%d %s %d\n",(int)this->stack.size(),dfa.type,dfa.name.c_str(),this->stack.top().state);
        //printf("lower:%d upper:%d, %d %s index:%d\n",state.lower,state.upper,token.type,token.str.c_str(),label_index);
        if (state.lower<=label_index&&label_index<state.upper){
            int t=state.accel[label_index-state.lower];
            if (t!=-1){
                //puts("in accel");
                assert(!stack.empty());
                if (t&(1<<7)){
                    int dst=t&((1<<7)-1);
                    int type=(t>>8)+NT_OFFSET;
                    //printf("nonterminal type:%d dst:%d\n",type,dst);
                    const auto &dfa=this->grammar->FindDFA(type);
                    Node *p=this->stack.top().parent;
                    Node *q=new Node(type,"",{},token.lineno,token.col_offset);
                    p->childs.push_back(q);
                    this->stack.top().state=dst;
                    this->stack.push({dfa.initial,dfa,q});
                    continue;
                } else{
                    int dst=t;
                    this->stack.top().state=dst;
                    Node *p=this->stack.top().parent;
                    Node *q=new Node(token.type,token.str,{},token.lineno,token.col_offset);
                    p->childs.push_back(q);
                    this->stack.top().state=dst;
                    for (;;){
                        const auto &state=this->stack.top().dfa.states[this->stack.top().state];
                        if (!(state.accept&&(int)state.arcs.size()==1)) break;
                        this->stack.pop();
                        if (this->stack.empty()){
                            return;
                        }
                    }
                    return;
                }
            }
        }
        assert(this->stack.top().dfa.states[this->stack.top().state].accept);
        this->stack.pop();
    }
}
Node *Parser::ParseFile(FILE *fp){
    this->Reset();

    auto tokenizer=new Tokenizer(fp); 
    for (;;){
        Token item=tokenizer->GetNextToken();
        //printf("item:%d \"%s\" %d %d\n",item.type,item.str.c_str(),item.lineno,item.col_offset);
        this->AddToken(item);
        if (this->stack.empty()) break;
    }
    return this->tree;
}

