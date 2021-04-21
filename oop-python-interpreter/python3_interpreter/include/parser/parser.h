#pragma once
#include "common.h"
#include "grammar.h"
#include "node.h"
#include "tokenizer.h"
class Parser{
    Grammar *grammar;
    int start;
    struct StackEntry{
        int state;
        const DFA &dfa;
        Node *parent;
        StackEntry(int _state,const DFA &_dfa,Node *_parent):state(_state),dfa(_dfa),parent(_parent){}
    };
    std::stack<StackEntry> stack;
    Node *tree=nullptr;
    void Reset();
    int classify(const Token &token) const;
    void AddToken(const Token &token);
    void GarbageCollection(Node *p);
public:
    Parser(Grammar *_grammar,int _start):grammar(_grammar),start(_start){}
    ~Parser();
    Node *ParseFile(FILE *fp);
};
