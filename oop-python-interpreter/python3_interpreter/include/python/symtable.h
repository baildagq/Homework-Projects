#pragma once
#include "common.h"
#include "python_ast.h"
#include "all_object.h"
enum{
    DEF_PARAM=1<<0,
    DEF_LOCAL=1<<1,
    USE=1<<2,
    DEF_FREE_CLASS=1<<3,
    DEF_NONLOCAL=1<<4,
    DEF_GLOBAL=1<<5,
};
const int DEF_BOUND=DEF_PARAM|DEF_LOCAL;
enum ScopeKind{
    LOCAL=1,
    FREE,
    GLOBAL_IMPLICIT,
    GLOBAL_EXPLICIT,
    CELL,
};
const int SCOPE_OFFSET=15;

enum BlockType{
    FunctionBlock,
    ClassBlock,
    ModuleBlock,
};
class SymTable;
class SymTableEntry{
public:
    std::string name;
    BlockType type;
    void *key;
    SymTable *table;
    int lineno;
    int col_offset;
    bool has_free=false;
    bool has_child_free=false;
    bool has_yield=false;
    std::map<std::string,int> symbols;
    std::vector<std::string> varnames;
    std::vector<SymTableEntry*> childs;
    SymTableEntry(const std::string &_name,BlockType _type,void *_key,SymTable *_table,int _lineno,int _col_offset);
    void AddDef(const std::string &str,int flag);
    void AnalyzeName(const std::string &name,int flag,std::map<std::string,int> *scopes,std::set<std::string> *bound,std::set<std::string> *local,std::set<std::string> *free,std::set<std::string> *global);
    void AnalyzeBlock(std::set<std::string> *bound,std::set<std::string> *free,std::set<std::string> *global);
};
class SymTable{
    std::stack<SymTableEntry*> stack;
    std::map<void*,SymTableEntry*> blocks;
public:
    void EnterBlock(const std::string &name,BlockType type,void *node,int lineno,int col_offset);
    void ExitBlock();
    void AddDef(const std::string &str,int flag);
    void visit_expr(expr_ty *expr);
    void visit_stmt(stmt_ty *stmt);
    void visit_mod(mod_ty *mod);
    SymTable(mod_ty *mod);

    SymTableEntry *Find(void *key);
};
