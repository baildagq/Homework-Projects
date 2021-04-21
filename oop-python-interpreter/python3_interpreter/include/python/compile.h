#pragma once
#include "python_ast.h"
#include "all_object.h"
#include "node.h"
#include "symtable.h"
class BasicBlock;
class Instruction{
public:
    int opcode;
    int arg=-1;
    int lineno=-1;
    BasicBlock *jump_block=nullptr;
    bool jump_abs=false;
    bool jump_rel=false;
    Instruction(int _opcode);
    Instruction(int _opcode,int _arg);
    Instruction(int _opcode,BasicBlock *_jump_block);
    int Size() const;
};
class BasicBlock{
public:
    BasicBlock *next=nullptr;
    std::vector<Instruction*> ins;
    bool has_return=false;
    int offset=-1;
    void AddOP(int opcode);
    void AddOP(int opcode,int arg);
    void AddJumpOP(int opcode,BasicBlock *block,bool absolute);
    int Size() const;
};
enum FrameBlockType{
    WHILE_LOOP,
    FOR_LOOP,
};
class FrameBlock{
public:
    FrameBlockType type;
    BasicBlock *enter_block;
    BasicBlock *exit_block;
    FrameBlock(FrameBlockType _type,BasicBlock *_enter_block,BasicBlock *_exit_block);
};
class CompilerUnit{
public:
    int lineno;
    int col_offset;
    SymTableEntry *ste;
    std::vector<BasicBlock*> blocks;
    BasicBlock *block;
    std::vector<FrameBlock*> frameblocks;
    std::map<Object*,int> consts;
    std::map<std::string,int> names;
    std::map<std::string,int> varnames;
    std::map<std::string,int> cellvars;
    std::map<std::string,int> freevars;
    CompilerUnit(SymTableEntry *_ste);
    int AddName(const std::string &str);
    int AddCellName(const std::string &str);
    int AddFreeName(const std::string &str);
    int AddConst(Object *ptr);
    void AddOP(int opcode);
    void AddOP(int opcode,int arg);
    void AddJumpOP(int opcode,BasicBlock *block,bool absolute);
};
class ByteCode{
    struct ByteCodeUnit{
        int opcode;
        int oparg;
        ByteCodeUnit(int _opcode,int _oparg):opcode(_opcode),oparg(_oparg){}
    };
public:
    std::vector<ByteCodeUnit*> codes;
    ByteCode();
    void AddOP(int opcode,int oparg); 
};
class ControlFlowGraph{
public:
    std::vector<BasicBlock*> order;
    std::set<BasicBlock*> vis;
    void DFS(BasicBlock *block);
    void CalcJump();
    ControlFlowGraph(BasicBlock *entryblock);
    ByteCode *GenByteCode();
};
class Compiler{
    SymTable *symtable;
    std::stack<CompilerUnit*> stack;
    void AddOP(int opcode);
    void AddOP(int opcode,int arg);
    void AddJumpOP(int opcode,BasicBlock *block,bool absolute);
    void AddNameOP(identifier_ty *id,expr_context_ty *ctx);
    void AddLoadConst(Object *o);
    std::vector<std::string> GetNames();
    std::vector<Object*> GetConsts();
    std::vector<std::string> GetVarNames();
    std::vector<std::string> GetFreeNames();
    std::vector<std::string> GetCellNames();
    std::vector<int> GetVarLocals();

    void MakeClosure(CodeObject *codeobject);

    BasicBlock *NewBlock();
    void EnterScope(void *key);
    void ExitScope();
    void AddBlock();
    void AddBlock(BasicBlock *block);
    void EnterFrameBlock(FrameBlockType type,BasicBlock *block,BasicBlock *exit);
    void ExitFrameBlock();
    void UnwindFrameBlock(FrameBlock *frameblock,bool preserve_tos);
    void UnwindAll(bool preserve_tos);
    void PopJumpIf(expr_ty *expr,BasicBlock *block,bool cond);

    CodeObject *Assemble(bool return_none);

    void visit_listcomp(expr_ty *expr,int index=0);

    void visit_expr(expr_ty *expr);
    void visit_stmt(stmt_ty *stmt);
    void visit_stmts(asdl_seq<stmt_ty*> *stmts);
    CodeObject *compile_mod(mod_ty *mod);
public:
    CodeObject *Compile(mod_ty *mod);
    CodeObject *Compile(Node *n);
    CodeObject *Compile(FILE *fp);
};
