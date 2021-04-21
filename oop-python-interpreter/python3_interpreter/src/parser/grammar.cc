#include "common.h"
#include "grammar.h"
#include "token.h"
#include "bitset.h"
inline bool IsAlpha(char c){
    return 'A'<=c&&c<='Z';
}
inline bool Isalpha(char c){
    return 'a'<=c&&c<='z';
}
int GetOneCharToken(const std::string str){
    if (str=="(") return LPAR;
    if (str==")") return RPAR;
    if (str=="[") return LSQB;
    if (str=="]") return RSQB;
    if (str==":") return COLON;
    if (str==",") return COMMA;
    if (str==";") return SEMI;
    if (str=="+") return PLUS;
    if (str=="-") return MINUS;
    if (str=="*") return STAR;
    if (str=="/") return SLASH;
    if (str=="|") return VBAR;
    if (str=="&") return AMPER;
    if (str=="<") return LESS;
    if (str==">") return GREATER;
    if (str=="=") return EQUAL;
    if (str==".") return DOT;
    if (str=="%") return PERCENT;
    if (str=="{") return LBRACE;
    if (str=="}") return RBRACE;
    if (str=="^") return CIRCUMFLEX;
    if (str=="~") return TILDE;
    if (str=="@") return AT;
    return -1;
}
int GetTwoCharToken(const std::string str){
    if (str=="==") return EQEQUAL;
    if (str=="!=") return NOTEQUAL;
    if (str=="<=") return LESSEQUAL;
    if (str=="<<") return LEFTSHIFT;
    if (str==">=") return GREATEREQUAL;
    if (str==">>") return RIGHTSHIFT;
    if (str=="+=") return PLUSEQUAL;
    if (str=="-=") return MINEQUAL;
    if (str=="->") return RARROW;
    if (str=="**") return DOUBLESTAR;
    if (str=="*=") return STAREQUAL;
    if (str=="//") return DOUBLESLASH;
    if (str=="/=") return SLASHEQUAL;
    if (str=="|=") return VBAREQUAL;
    if (str=="%=") return PERCENTEQUAL;
    if (str=="&=") return AMPEREQUAL;
    if (str=="^=") return CIRCUMFLEXEQUAL;
    if (str=="@=") return ATEQUAL;
    return -1;
}
int GetThreeCharToken(const std::string str){
    if (str=="<<=") return LEFTSHIFTEQUAL;
    if (str==">>=") return RIGHTSHIFTEQUAL;
    if (str=="**=") return DOUBLESTAREQUAL;
    if (str=="//=") return DOUBLESLASHEQUAL;
    if (str=="...") return ELLIPSIS;
    return -1;
}
void Grammar::TranslateLabels(Label &label){
    //printf("label: %d %s\n",label.type,label.str.c_str());
    if (label.type==NAME){
        for (int i=0;i<(int)this->dfas.size();i++) if (label.str==this->dfas[i].name){
            label.type=this->dfas[i].type;
            label.str="";
            return;
        }
        int token_index=TokenMap(label.str);
        if (token_index>=0){
            label.type=token_index;
            label.str="";
            return;
        }
        assert(false);
    } else if (label.type==STRING){
        if (IsAlpha(label.str[1])||Isalpha(label.str[1])||label.str[1]=='_'){
            label.type=NAME;
            label.str=label.str.substr(1,(int)label.str.size()-2);
        } else if (label.str[2]==label.str[0]){
            int type=GetOneCharToken(label.str.substr(1,1));
            assert(type>=0);
            label.type=type;
            label.str="";
        } else if (label.str[3]==label.str[0]){
            int type=GetTwoCharToken(label.str.substr(1,2));
            assert(type>=0);
            label.type=type;
            label.str="";
        } else if (label.str[4]==label.str[0]){
            int type=GetThreeCharToken(label.str.substr(1,3));
            label.type=type;
            label.str="";
        } else{
            assert(false);
        }
    } else{
        assert(false);
    }
}
void Grammar::TranslateLabels(){
    for (int i=0;i<(int)this->labels.size();i++){
        if (i==EMPTY) continue;
        this->TranslateLabels(this->labels[i]);
    }
}
void Grammar::CalcFirstSet(int status[],int i){
    assert(status[i]!=-1);
    if (status[i]==1) return;
    this->dfas[i].first.Reset();
    //printf("%d init:%d\n",i,this->dfas[i].initial);
    status[i]=-1;
    for (const auto &arc:this->dfas[i].states[this->dfas[i].initial].arcs){
        const auto &label=this->labels[arc.label];
        //printf("%d %d str:%s\n",i,label.type,label.str.c_str());
        if (IsNonTerminal(label.type)){
            CalcFirstSet(status,this->FindDFAIndex(label.type));
            this->dfas[i].first|=this->FindDFA(label.type).first;
        } else if (IsTerminal(label.type)){
            this->dfas[i].first.AddBit(arc.label);
        }
    }
    status[i]=1;
}
void Grammar::CalcFirstSet(){
    int *status=new int[(int)this->dfas.size()];
    memset(status,0,sizeof(*status)*(int)this->dfas.size());
    for (int i=0;i<(int)this->dfas.size();i++){
        this->CalcFirstSet(status,i);
    }
    delete[] status;
}
int Grammar::FindDFAIndex(int type) const{
    int index=type-NT_OFFSET;
    assert(this->dfas[index].type==type);
    return index;
}
const DFA &Grammar::FindDFA(int type) const{
    const DFA &dfa=dfas[this->FindDFAIndex(type)];
    assert(dfa.type==type);
    return dfa;
}
void Grammar::AddAccelerator(){
    if (this->is_accel) return;
    for (auto &dfa:this->dfas) for (auto &state:dfa.states){
        //printf("dfa:%s\n",dfa.name.c_str());
        int *accel=new int[(int)this->labels.size()];
        assert(accel!=nullptr);
        memset(accel,-1,sizeof(*accel)*(int)this->labels.size());
        for (const auto &arc:state.arcs){
            assert(arc.dst<(1<<7));
            const auto &label=this->labels[arc.label];
            //printf("label:%d %d %s\n",arc.label,label.type,label.str.c_str());
            if (IsNonTerminal(label.type)){
                assert(label.type-NT_OFFSET<(1<<7));
                const auto &dfa=this->FindDFA(label.type);
                //printf("nonterminal %s\n",dfa.name.c_str());
                for (int i=0;i<(int)this->labels.size();i++){
                    if (dfa.first.TestBit(i)){
                        //printf("testbit %d %d %s\n",i,labels[i].type,labels[i].str.c_str());
                        int t=accel[i];
                        accel[i]=arc.dst|(1<<7)|((label.type-NT_OFFSET)<<8);
                        assert(t==-1||t==accel[i]);
                    }
                }
            } else if (arc.label==EMPTY){
                state.accept=true;
            } else if (0<=arc.label&&arc.label<(int)this->labels.size()){
                accel[arc.label]=arc.dst;
            }
        }
        state.lower=0;
        state.upper=(int)this->labels.size();
        for (;state.lower<(int)this->labels.size()&&accel[state.lower]==-1;) state.lower++;
        for (;state.upper>state.lower&&accel[state.upper-1]==-1;) state.upper--;
        if (state.lower<state.upper){
            state.accel=new int[state.upper-state.lower];
            assert(state.accel!=nullptr);
            memcpy(state.accel,accel+state.lower,sizeof(*state.accel)*(state.upper-state.lower));
        }
        delete[] accel;
    }
    this->is_accel=true;
}
void Grammar::PrintDFAs(FILE *fp) const{
    for (int i=0;i<(int)this->dfas.size();i++){
        const auto &dfa=this->dfas[i];
        for (int j=0;j<(int)dfa.states.size();j++){
            fprintf(fp,"std::vector<Arc> arcs_%d_%d={",i,j);
            for (int k=0;k<(int)dfa.states[j].arcs.size();k++){
                fprintf(fp,"{%d,%d},",dfa.states[j].arcs[k].label,dfa.states[j].arcs[k].dst);
            }
            fprintf(fp,"};\n");
        }
        fprintf(fp,"std::vector<DFAState> states_%d={",i);
        for (int j=0;j<(int)dfa.states.size();j++){
            fprintf(fp,"arcs_%d_%d,",i,j);
        }
        fprintf(fp,"};\n");
        fprintf(fp,"DFA dfa_%d={%d,\"%s\",%d,states_%d,{%d,\"%s\"}};\n",i,dfa.type,dfa.name.c_str(),dfa.initial,i,(int)dfa.first.ToString().size(),dfa.first.ToString().c_str());
        fprintf(fp,"\n");
    }
    fprintf(fp,"std::vector<DFA> dfas={\n");
    for (int i=0;i<(int)this->dfas.size();i++){
        fprintf(fp,"    dfa_%d,\n",i);
    }
    fprintf(fp,"};\n");
}
void Grammar::PrintLabels(FILE *fp) const{
    fprintf(fp,"std::vector<Label> labels={\n");
    for (const auto &label:this->labels)
        fprintf(fp,"    {%d,\"%s\"},\n",label.type,label.str.c_str());
    fprintf(fp,"};\n");
}
void Grammar::PrintGrammar(FILE *fp) const{
    fprintf(fp,"/*.cc generated by pgen*/\n\n");
    fprintf(fp,"#include \"grammar.h\"\n");
    fprintf(fp,"#include \"token.h\"\n");
    fprintf(fp,"#include \"bitset.h\"\n");
    fprintf(fp,"#include \"graminit.h\"\n");
    fprintf(fp,"\n");
    PrintLabels(fp);
    PrintDFAs(fp);
    fprintf(fp,"Grammar grammar(labels,dfas);\n");

    fprintf(fp,"\n");
    fprintf(fp,"Grammar *GetGrammar(){\n");
    fprintf(fp,"    return &grammar;\n");
    fprintf(fp,"}\n");

    fprintf(fp,"std::string NonTerminalMap(int i){\n");
    for (const auto &dfa:this->dfas){
        fprintf(fp,"    if (i==%d) return \"%s\";\n",dfa.type,dfa.name.c_str());
    }
    fprintf(fp,"    assert(false);\n");
    fprintf(fp,"}\n");
}
void Grammar::PrintNonterminals(FILE *fp) const{
    fprintf(fp,"/*.h generated by pgen*/\n\n");
    fprintf(fp,"#include \"common.h\"\n");
    fprintf(fp,"#include \"grammar.h\"\n");
    fprintf(fp,"enum{\n");
    for (const auto &dfa:this->dfas){
        fprintf(fp,"    %s=%d,\n",dfa.name.c_str(),dfa.type);
    }
    fprintf(fp,"};\n");
    fprintf(fp,"Grammar *GetGrammar();\n");
    fprintf(fp,"std::string NonTerminalMap(int i);");
}
